/*
 * ========================================================================
 * PHP AMF3 encoding/decoding extension
 * Work started by Arseny Vakhrushev on 11 Jan 2010
 * Copyright (C) 2010 IT Territory, LLC. http://it-territory.ru/
 * ========================================================================
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * Please read the LICENSE file for license details
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <php.h>
#include <Zend/zend_interfaces.h>
#include <ext/standard/info.h>
#include <ext/date/php_date.h>
#include "php_amf3.h"

ZEND_BEGIN_ARG_INFO_EX(arginfo_amf3_encode, 0, 0, 1)
	ZEND_ARG_INFO(0, value)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_amf3_decode, 0, 0, 1)
	ZEND_ARG_INFO(0, data)
	ZEND_ARG_INFO(1, count)
ZEND_END_ARG_INFO()

static const zend_function_entry amf3_functions[] = {
	PHP_FE(amf3_encode, arginfo_amf3_encode)
	PHP_FE(amf3_decode, arginfo_amf3_decode)
	PHP_FE_END
};

static zend_class_entry *(*sxe_get_element_class_entry)();

static PHP_MINFO_FUNCTION(amf3)
{
	php_info_print_table_start();
	php_info_print_table_header(2, "AMF3 support", "enabled");
	php_info_print_table_row(2, "Version", PHP_AMF3_VERSION);
	php_info_print_table_row(2, "Build Date", __DATE__ " " __TIME__);
	php_info_print_table_row(2, "XML support", sxe_get_element_class_entry ? "enabled" : "disabled");
	php_info_print_table_end();
}

static PHP_MINIT_FUNCTION(amf3)
{
	sxe_get_element_class_entry = (zend_class_entry * (*)())DL_FETCH_SYMBOL(NULL, "sxe_get_element_class_entry");

	return SUCCESS;
}

zend_module_entry amf3_module_entry = {
	STANDARD_MODULE_HEADER,
	"amf3",
	amf3_functions,
	PHP_MINIT(amf3),
	NULL,
	NULL,
	NULL,
	PHP_MINFO(amf3),
	PHP_AMF3_VERSION,
	STANDARD_MODULE_PROPERTIES
};

#ifdef COMPILE_DL_AMF3
ZEND_GET_MODULE(amf3)
#endif

/* ============================================================================================================ */

#define AMF3_MAX_INT     268435455 //  (2^28)-1
#define AMF3_MIN_INT    -268435456 // -(2^28)

typedef enum amf3_type_e amf3_type_t;
typedef struct amf3_chunk_s amf3_chunk_t;
typedef struct amf3_env_s amf3_env_t;
typedef struct amf3_traits_s amf3_traits_t;

enum amf3_type_e {
	AMF3_UNDEFINED     = 0x00,
	AMF3_NULL          = 0x01,
	AMF3_FALSE         = 0x02,
	AMF3_TRUE          = 0x03,
	AMF3_INTEGER       = 0x04,
	AMF3_DOUBLE        = 0x05,
	AMF3_STRING        = 0x06,
	AMF3_XMLDOC        = 0x07, // only decoding
	AMF3_DATE          = 0x08,
	AMF3_ARRAY         = 0x09,
	AMF3_OBJECT        = 0x0a, // without externalizable
	AMF3_XML           = 0x0b,
	AMF3_BYTEARRAY     = 0x0c, // only decoding
	AMF3_VECTOR_INT    = 0x0d, // no support
	AMF3_VECTOR_UINT   = 0x0e, // no support
	AMF3_VECTOR_DOUBLE = 0x0f, // no support
	AMF3_VECTOR_OBJECT = 0x10, // no support
	AMF3_DICTIONARY    = 0x11, // no support
};

struct amf3_chunk_s {
	char             buf[1000]; // some suitable chunk size to store both small and big buffers well enough and to fit 1Kb memory block
	int              size;
	amf3_chunk_t     *next;
};

struct amf3_env_s {
	HashTable        strs;
	HashTable        objs;
	HashTable        traits;
};

struct amf3_traits_s {
	zend_class_entry **ce;
	int              memberCount;
	char             **members;
	int              *memberLengths;
	int              dynamic;
};

static int amf3_encodeVal(amf3_chunk_t **chunk, zval *val, amf3_env_t *env TSRMLS_DC);
static int amf3_decodeVal(zval **val, const char *data, int pos, int size, amf3_env_t *env TSRMLS_DC);

/* ============================================================================================================ */


static amf3_chunk_t *amf3_initChunk() {
	amf3_chunk_t *chunk;

	chunk = (amf3_chunk_t *)ecalloc(1, sizeof(*chunk));
	if (!chunk) {
		return NULL;
	}
	return chunk;
}

static amf3_chunk_t *amf3_appendChunk(amf3_chunk_t *chunk, const char *buf, int size) {
	if (!chunk) {
		return NULL;
	}
	for ( ;; ) {
		int avail = sizeof(chunk->buf) - chunk->size;
		if (avail >= size) {
			break;
		}
		memcpy(chunk->buf + chunk->size, buf, avail);
		chunk->size += avail;
		buf += avail;
		size -= avail;
		if (!chunk->next) {
			chunk->next = amf3_initChunk();
		}
		chunk = chunk->next;
	}
	memcpy(chunk->buf + chunk->size, buf, size);
	chunk->size += size;
	return chunk;
}

static void amf3_freeChunk(amf3_chunk_t *chunk, char *buf) {
	amf3_chunk_t *next;
	while (chunk) {
		next = chunk->next;
		memcpy(buf, chunk->buf, chunk->size);
		buf += chunk->size;
		efree(chunk);
		chunk = next;
	}
}

static int amf3_getStrIdx(amf3_env_t *env, const char *str, int len) {
	if (len <= 0) {
		return -1; // empty string is never sent by reference
	}
	if (len > AMF3_MAX_INT) {
		len = AMF3_MAX_INT;
	}
	int *oldIdx;
	if (zend_hash_find(&env->strs, str, len, (void **)&oldIdx) == SUCCESS) {
		return *oldIdx;
	}
	int newIdx = zend_hash_num_elements(&env->strs);
	if (newIdx <= AMF3_MAX_INT) {
		zend_hash_add(&env->strs, str, len, &newIdx, sizeof(newIdx), NULL);
	}
	return -1;
}

static int amf3_getObjIdx(amf3_env_t *env, zval *val) {
	int *oldIdx;
	if (Z_ISREF_P(val) && (zend_hash_find(&env->objs, (char *)&val, sizeof(val), (void **)&oldIdx) == SUCCESS)) {
		return *oldIdx;
	}
	int newIdx = zend_hash_num_elements(&env->objs);
	if (newIdx <= AMF3_MAX_INT) {
		zend_hash_add(&env->objs, (char *)&val, sizeof(val), &newIdx, sizeof(newIdx), NULL);
	}
	return -1;
}

static int amf3_getTraitsIdx(amf3_env_t *env, zend_class_entry *ce) {
	int *oldIdx;
	if (zend_hash_find(&env->traits, (char *)&ce, sizeof(ce), (void **)&oldIdx) == SUCCESS) {
		return *oldIdx;
	}
	int newIdx = zend_hash_num_elements(&env->traits);
	if (newIdx <= AMF3_MAX_INT) {
		zend_hash_add(&env->traits, (char *)&ce, sizeof(ce), &newIdx, sizeof(newIdx), NULL);
	}
	return -1;
}

static zval *amf3_getRef(HashTable *ht, int idx) {
	zval **val;
	if (zend_hash_index_find(ht, idx, (void **)&val) == FAILURE) {
		return NULL;
	}
	Z_ADDREF_PP(val);
	return *val;
}

static void amf3_putRef(HashTable *ht, zval *val) {
	Z_ADDREF_P(val);
	zend_hash_index_update(ht, zend_hash_num_elements(ht), &val, sizeof(val), NULL);
}


/* ============================================================================================================ */


static int amf3_encodeChar(amf3_chunk_t **chunk, char c) {
	*chunk = amf3_appendChunk(*chunk, &c, 1);
	return 1;
}

static int amf3_encodeU29(amf3_chunk_t **chunk, int val) {
	char buf[4];
	int pos;
	val &= 0x1fffffff;
	if (val <= 0x7f) {
		buf[0] = val;
		pos = 1;
	} else if (val <= 0x3fff) {
		buf[1] = val & 0x7f;
		val >>= 7;
		buf[0] = val | 0x80;
		pos = 2;
	} else if (val <= 0x1fffff) {
		buf[2] = val & 0x7f;
		val >>= 7;
		buf[1] = val | 0x80;
		val >>= 7;
		buf[0] = val | 0x80;
		pos = 3;
	} else {
		buf[3] = val;
		val >>= 8;
		buf[2] = val | 0x80;
		val >>= 7;
		buf[1] = val | 0x80;
		val >>= 7;
		buf[0] = val | 0x80;
		pos = 4;
	}
	*chunk = amf3_appendChunk(*chunk, buf, pos);
	return pos;
}

static int amf3_decodeU29(int *val, const char *buf, int size) {
	int pos = 0, res = 0, tmp;
	do {
		if (pos >= size) {
			return -1;
		}
		tmp = buf[pos];
		if (pos == 3) {
			res <<= 8;
			res |= tmp & 0xff;
		} else {
			res <<= 7;
			res |= tmp & 0x7f;
		}
	} while ((++pos < 4) && (tmp & 0x80));
	*val = res;
	return pos;
}

static int amf3_encodeDouble(amf3_chunk_t **chunk, double val) {
	union {
		double d;
		zend_ulong64 l;
	} u = { val };
	zend_ulong64 l = u.l;
	char buf[8];
	int i;
	for (i = 0; i < 8; ++i) {
		buf[7 - i] = l;
		l >>= 8;
	}
	*chunk = amf3_appendChunk(*chunk, buf, 8);
	return 8;
}

static int amf3_decodeDouble(double *val, const char *buf, int size) {
	if (size < 8) {
		return -1;
	}
	zend_ulong64 l = 0;
	int i;
	for (i = 0; i < 8; ++i) {
		l <<= 8;
		l |= buf[i] & 0xff;
	}
	union {
		zend_ulong64 l;
		double d;
	} u = { l };
	*val = u.d;
	return 8;
}

static int amf3_encodeStr(amf3_chunk_t **chunk, const char *str, int len, amf3_env_t *env) {
	int pos = 0, idx = amf3_getStrIdx(env, str, len);
	if (idx >= 0) {
		pos += amf3_encodeU29(chunk, idx << 1); // encode as a reference
	} else {
		if (len > AMF3_MAX_INT) {
			len = AMF3_MAX_INT;
		}
		pos += amf3_encodeU29(chunk, (len << 1) | 1) + len;
		*chunk = amf3_appendChunk(*chunk, str, len);
	}
	return pos;
}

static int amf3_decodeStr(const char **str, unsigned int *len, const char *buf, int size, amf3_env_t *env) {
	int pfx, pos = amf3_decodeU29(&pfx, buf, size);
	if (pos < 0) {
		return -1;
	}
	if (!(pfx & 1)) { // decode as a reference
		zval **val;
		if (zend_hash_index_find(&env->strs, pfx >> 1, (void **)&val) == FAILURE) {
			return -1;
		}
		*str = Z_STRVAL_PP(val);
		*len = Z_STRLEN_PP(val);
	} else {
		pfx >>= 1;
		if ((pfx < 0) || ((pos + pfx) > size)) {
			return -1;
		}
		*str = buf + pos;
		*len = pfx;
		if (pfx > 0) { // empty string is never sent by reference
			zval *val;
			ALLOC_INIT_ZVAL(val);
			ZVAL_STRINGL(val, buf + pos, pfx, 1);
			zend_hash_index_update(&env->strs, zend_hash_num_elements(&env->strs), &val, sizeof(val), NULL);
		}
		pos += pfx;
	}
	return pos;
}

static int amf3_encodeDate(amf3_chunk_t **chunk, zval *val, amf3_env_t *env TSRMLS_DC) {
	int pos = amf3_encodeChar(chunk, AMF3_DATE);
	int idx = amf3_getObjIdx(env, val);
	if (idx >= 0) {
		pos += amf3_encodeU29(chunk, idx << 1); // encode as a reference
	} else {
		zval *return_value;
		double date;

		zend_call_method_with_0_params(&val, NULL, NULL, "getTimestamp", &return_value);

		pos += amf3_encodeU29(chunk, 1);
		date = Z_LVAL_P(return_value);
		pos += amf3_encodeDouble(chunk, date * 1000.0);

		FREE_ZVAL(return_value);
	}
	return pos;
}

static int amf3_encodeArray(amf3_chunk_t **chunk, zval *val, amf3_env_t *env TSRMLS_DC) {
	int pos = amf3_encodeChar(chunk, AMF3_ARRAY);
	int idx = amf3_getObjIdx(env, val);
	if (idx >= 0) {
		pos += amf3_encodeU29(chunk, idx << 1); // encode as a reference
	} else {
		HashTable *ht = Z_ARRVAL_P(val);
		HashPosition hp;
		zval **hv;
		char *key, keyBuf[22];
		int keyType, num = 0;
		uint keyLen;
		ulong idx;
		for (zend_hash_internal_pointer_reset_ex(ht, &hp);; zend_hash_move_forward_ex(ht, &hp)) {
			keyType = zend_hash_get_current_key_ex(ht, &key, &keyLen, &idx, 0, &hp);
			if ((keyType != HASH_KEY_IS_LONG) || (idx != (ulong)num)) {
				break;
			}
			++num;
		}
		if (num == zend_hash_num_elements(ht)) { // sequence of values with integer indexes starting from zero
			if (num > AMF3_MAX_INT) {
				num = AMF3_MAX_INT;
			}
			pos += amf3_encodeU29(chunk, (num << 1) | 1); // dense part size
			pos += amf3_encodeChar(chunk, 0x01); // end of associative part
			for (zend_hash_internal_pointer_reset_ex(ht, &hp); (num-- > 0) && (zend_hash_get_current_data_ex(ht, (void **)&hv, &hp) == SUCCESS); zend_hash_move_forward_ex(ht, &hp)) {
				pos += amf3_encodeVal(chunk, *hv, env TSRMLS_CC);
			}
		} else { // associative array with mixed keys
			pos += amf3_encodeChar(chunk, 0x01); // empty dense part
			for (zend_hash_internal_pointer_reset_ex(ht, &hp); zend_hash_get_current_data_ex(ht, (void **)&hv, &hp) == SUCCESS; zend_hash_move_forward_ex(ht, &hp)) {
				keyType = zend_hash_get_current_key_ex(ht, &key, &keyLen, &idx, 0, &hp);
				if (keyType == HASH_KEY_IS_STRING) {
					if (keyLen <= 1) {
						continue; // empty keys can't be represented in AMF3
					}
					pos += amf3_encodeStr(chunk, key, keyLen - 1, env);
				} else if (keyType == HASH_KEY_IS_LONG) {
					keyLen = sprintf(keyBuf, "%ld", idx);
					pos += amf3_encodeStr(chunk, keyBuf, keyLen, env);
				} else {
					continue;
				}
				pos += amf3_encodeVal(chunk, *hv, env TSRMLS_CC);
			}
			pos += amf3_encodeChar(chunk, 0x01); // end of associative part
		}
	}
	return pos;
}

static int amf3_encodeXml(amf3_chunk_t **chunk, zval *val, amf3_env_t *env TSRMLS_DC) {
	int pos = amf3_encodeChar(chunk, AMF3_XML);
	int idx = amf3_getObjIdx(env, val);
	if (idx >= 0) {
		pos += amf3_encodeU29(chunk, idx << 1); // encode as a reference
	} else {
		zval *xml;
		int xmlLen;

		zend_call_method_with_0_params(&val, NULL, NULL, "asXML", &xml);

		xmlLen = Z_STRLEN_P(xml);
		if (xmlLen > AMF3_MAX_INT) {
			xmlLen = AMF3_MAX_INT;
		}
		pos += amf3_encodeU29(chunk, (xmlLen << 1) | 1);
		*chunk = amf3_appendChunk(*chunk, Z_STRVAL_P(xml), xmlLen);
		pos += xmlLen;

		zval_dtor(xml);
		FREE_ZVAL(xml);
	}
	return pos;
}

static int amf3_encodeObjectTraits(amf3_chunk_t **chunk, zval *val, zend_class_entry *ce, amf3_env_t *env TSRMLS_DC) {
	int pos = 0;
	int idx = amf3_getTraitsIdx(env, ce);
	if (idx >= 0) {
		pos += amf3_encodeU29(chunk, (idx << 2) | 1); // encode as a reference
	} else {
		if (ce == zend_standard_class_def) {
			pos += amf3_encodeU29(chunk, 0x0b);
			pos += amf3_encodeChar(chunk, 0x01); // empty class name
		} else {
			HashTable *ht = Z_OBJPROP_P(val);
			HashPosition hp;
			zval **hv;
			char *key;
			int keyType;
			uint keyLen;
			ulong idx;
			int members = 0;

			// count number of properties
			for (zend_hash_internal_pointer_reset_ex(ht, &hp); zend_hash_get_current_data_ex(ht, (void **)&hv, &hp) == SUCCESS; zend_hash_move_forward_ex(ht, &hp)) {
				keyType = zend_hash_get_current_key_ex(ht, &key, &keyLen, &idx, 0, &hp);
				if (keyType == HASH_KEY_IS_STRING) {
					if (keyLen <= 1 || key[0] == 0) {
						continue; // skip empty key and private/protected properties
					}
					if (!zend_hash_quick_exists(&ce->properties_info, key, keyLen, zend_get_hash_value(key, keyLen))) {
						continue; // skip dynamic properties
					}
					members++;
				}
			}

			pos += amf3_encodeU29(chunk, (members << 4) | 0x0b);
			pos += amf3_encodeStr(chunk, ce->name, ce->name_length, env);

			// write property names
			for (zend_hash_internal_pointer_reset_ex(ht, &hp); zend_hash_get_current_data_ex(ht, (void **)&hv, &hp) == SUCCESS; zend_hash_move_forward_ex(ht, &hp)) {
				keyType = zend_hash_get_current_key_ex(ht, &key, &keyLen, &idx, 0, &hp);
				if (keyType == HASH_KEY_IS_STRING) {
					if (keyLen <= 1 || key[0] == 0) {
						continue; // skip empty key and private/protected properties
					}
					if (!zend_hash_quick_exists(&ce->properties_info, key, keyLen, zend_get_hash_value(key, keyLen))) {
						continue; // skip dynamic properties
					}
					pos += amf3_encodeStr(chunk, key, keyLen - 1, env);
				}
			}
		}
	}

	return pos;
}

static int amf3_encodeObject(amf3_chunk_t **chunk, zval *val, amf3_env_t *env TSRMLS_DC) {
	int pos = 0, idx;
	zend_class_entry *ce = Z_OBJCE_P(val);

	if (instanceof_function(ce, php_date_get_date_ce() TSRMLS_CC)) {
		pos += amf3_encodeDate(chunk, val, env TSRMLS_CC);
		return pos;
	} else if(instanceof_function(ce, php_date_get_immutable_ce() TSRMLS_CC)) {
		pos += amf3_encodeDate(chunk, val, env TSRMLS_CC);
		return pos;
	} else if (sxe_get_element_class_entry && instanceof_function(ce, sxe_get_element_class_entry() TSRMLS_CC)) {
		pos += amf3_encodeXml(chunk, val, env TSRMLS_CC);
		return pos;
	}

	pos = amf3_encodeChar(chunk, AMF3_OBJECT);
	idx = amf3_getObjIdx(env, val);
	if (idx >= 0) {
		pos += amf3_encodeU29(chunk, idx << 1); // encode as a reference
	} else {
		HashTable *ht = Z_OBJPROP_P(val);
		HashPosition hp;
		zval **hv;
		char *key, keyBuf[22];
		int keyType;
		uint keyLen;
		ulong idx;

		pos += amf3_encodeObjectTraits(chunk, val, ce, env TSRMLS_CC);
		if (ce == zend_standard_class_def) { // encode as dynamic anonymous object
			for (zend_hash_internal_pointer_reset_ex(ht, &hp); zend_hash_get_current_data_ex(ht, (void **)&hv, &hp) == SUCCESS; zend_hash_move_forward_ex(ht, &hp)) {
				keyType = zend_hash_get_current_key_ex(ht, &key, &keyLen, &idx, 0, &hp);
				if (keyType == HASH_KEY_IS_STRING) {
					if (keyLen <= 1 || key[0] == 0) {
						continue; // skip empty key and private/protected properties
					}
					pos += amf3_encodeStr(chunk, key, keyLen - 1, env);
					pos += amf3_encodeVal(chunk, *hv, env TSRMLS_CC);
				} else if (keyType == HASH_KEY_IS_LONG) {
					// arrays with integer indexes when cast to object produce
					// objects with integer property names
					keyLen = sprintf(keyBuf, "%ld", idx);
					pos += amf3_encodeStr(chunk, keyBuf, keyLen, env);
					pos += amf3_encodeVal(chunk, *hv, env TSRMLS_CC);
				}
			}

			pos += amf3_encodeChar(chunk, 0x01); // end of dynamic members
		} else { // encode as typed object
			// write property values
			for (zend_hash_internal_pointer_reset_ex(ht, &hp); zend_hash_get_current_data_ex(ht, (void **)&hv, &hp) == SUCCESS; zend_hash_move_forward_ex(ht, &hp)) {
				keyType = zend_hash_get_current_key_ex(ht, &key, &keyLen, &idx, 0, &hp);
				if (keyType == HASH_KEY_IS_STRING) {
					if (keyLen <= 1 || key[0] == 0) {
						continue; // skip empty key and private/protected properties
					}
					if (!zend_hash_quick_exists(&ce->properties_info, key, keyLen, zend_get_hash_value(key, keyLen))) {
						continue; // skip dynamic properties
					}
					pos += amf3_encodeVal(chunk, *hv, env TSRMLS_CC);
				}
			}

			for (zend_hash_internal_pointer_reset_ex(ht, &hp); zend_hash_get_current_data_ex(ht, (void **)&hv, &hp) == SUCCESS; zend_hash_move_forward_ex(ht, &hp)) {
				keyType = zend_hash_get_current_key_ex(ht, &key, &keyLen, &idx, 0, &hp);
				if (keyType == HASH_KEY_IS_STRING) {
					if (keyLen <= 1 || key[0] == 0) {
						continue; // skip empty key and private/protected properties
					}
					if (zend_hash_quick_exists(&ce->properties_info, key, keyLen, zend_get_hash_value(key, keyLen))) {
						continue;
					}
					pos += amf3_encodeStr(chunk, key, keyLen - 1, env);
					pos += amf3_encodeVal(chunk, *hv, env TSRMLS_CC);
				}
			}

			pos += amf3_encodeChar(chunk, 0x01); // end of dynamic members
		}
	}
	return pos;
}

static int amf3_encodeVal(amf3_chunk_t **chunk, zval *val, amf3_env_t *env TSRMLS_DC) {
	int pos = 0;
	switch (Z_TYPE_P(val)) {
		case IS_NULL:
			pos += amf3_encodeChar(chunk, AMF3_NULL);
			break;
		case IS_BOOL:
			pos += amf3_encodeChar(chunk, Z_LVAL_P(val) ? AMF3_TRUE : AMF3_FALSE);
			break;
		case IS_LONG:
			if ((Z_LVAL_P(val) < AMF3_MIN_INT) || (Z_LVAL_P(val) > AMF3_MAX_INT)) {
				pos += amf3_encodeChar(chunk, AMF3_DOUBLE);
				pos += amf3_encodeDouble(chunk, Z_LVAL_P(val));
			} else {
				pos += amf3_encodeChar(chunk, AMF3_INTEGER);
				pos += amf3_encodeU29(chunk, Z_LVAL_P(val));
			}
			break;
		case IS_DOUBLE:
			pos += amf3_encodeChar(chunk, AMF3_DOUBLE);
			pos += amf3_encodeDouble(chunk, Z_DVAL_P(val));
			break;
		case IS_STRING:
			pos += amf3_encodeChar(chunk, AMF3_STRING);
			pos += amf3_encodeStr(chunk, Z_STRVAL_P(val), Z_STRLEN_P(val), env);
			break;
		case IS_ARRAY:
			pos += amf3_encodeArray(chunk, val, env TSRMLS_CC);
			break;
		case IS_OBJECT:
			pos += amf3_encodeObject(chunk, val, env TSRMLS_CC);
			break;
		default:
			php_error_docref(NULL TSRMLS_CC, E_WARNING, "Unable to encode unsupported value type");
			break;
	}
	return pos;
}

static void amf3_initVal(zval **val) {
	if (*val) {
		zval_dtor(*val);
		ZVAL_NULL(*val);
	} else {
		ALLOC_INIT_ZVAL(*val);
	}
}

static int amf3_decodeArray(zval **val, const char *data, int pos, int size, amf3_env_t *env TSRMLS_DC) {
	int oldPos = pos;
	int pfx, res = amf3_decodeU29(&pfx, data + pos, size - pos);
	if (res < 0) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Can't decode array prefix at position %d", pos);
		return -1;
	}
	pos += res;
	if (!(pfx & 1)) { // decode as a reference
		*val = amf3_getRef(&env->objs, pfx >> 1);
		if (!*val) {
			php_error_docref(NULL TSRMLS_CC, E_WARNING, "Missing array reference index at position %d", pos - res);
			return -1;
		}
		Z_SET_ISREF_PP(val);
	} else {
		pfx >>= 1;
		if ((pfx < 0) || ((pos + pfx) > size)) {
			php_error_docref(NULL TSRMLS_CC, E_WARNING, "Invalid dense array portion size at position %d", pos - res);
			return -1;
		}
		amf3_initVal(val);
		array_init(*val);
		amf3_putRef(&env->objs, *val);
		const char *key;
		char keyBuf[64];
		unsigned int keyLen;
		zval *hv;
		for ( ;; ) { // associative array portion
			res = amf3_decodeStr(&key, &keyLen, data + pos, size - pos, env);
			if (res < 0) {
				php_error_docref(NULL TSRMLS_CC, E_WARNING, "Can't decode array key at position %d", pos);
				return -1;
			}
			pos += res;
			if (!keyLen) {
				break;
			}

			hv = 0;
			res = amf3_decodeVal(&hv, data, pos, size, env TSRMLS_CC);
			if (hv) { // need a trailing \0 in the key buffer to do a proper call to 'add_assoc_zval_ex'
				if (keyLen < sizeof(keyBuf)) {
					memcpy(keyBuf, key, keyLen);
					keyBuf[keyLen] = 0;
					add_assoc_zval_ex(*val, keyBuf, keyLen + 1, hv);
				} else {
					char *tmpBuf = (char *)emalloc(keyLen + 1);
					memcpy(tmpBuf, key, keyLen);
					tmpBuf[keyLen] = 0;
					add_assoc_zval_ex(*val, tmpBuf, keyLen + 1, hv);
					efree(tmpBuf);
				}
			}
			if (res < 0) {
				return -1; // nested error
			}
			pos += res;
		}
		while (pfx-- > 0) {
			hv = 0;
			res = amf3_decodeVal(&hv, data, pos, size, env TSRMLS_CC);
			if (hv) {
				add_next_index_zval(*val, hv);
			}
			if (res < 0) {
				return -1; // nested error
			}
			pos += res;
		}
	}
	return pos - oldPos;
}

static int amf3_decodeObject(zval **val, const char *data, int pos, int size, amf3_env_t *env TSRMLS_DC) {
	int oldPos = pos;
	int pfx, res = amf3_decodeU29(&pfx, data + pos, size - pos);
	if (res < 0) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Can't decode object prefix at position %d", pos);
		return -1;
	}
	pos += res;
	if (!(pfx & 1)) { // decode as a reference
		*val = amf3_getRef(&env->objs, pfx >> 1);
		if (!*val) {
			php_error_docref(NULL TSRMLS_CC, E_WARNING, "Missing object reference index at position %d", pos - res);
			return -1;
		}
		Z_SET_ISREF_PP(val);
	} else {
		amf3_traits_t *traits;
		int members;
		const char *key;
		char keyBuf[64];
		unsigned int keyLen;
		zval *prop;

		if (!(pfx & 2)) { // decode traits as a reference
			ulong idx = pfx >> 2;
			amf3_traits_t **found;
			if (zend_hash_index_find(&env->traits, idx, (void **)&found) == FAILURE) {
				php_error_docref(NULL TSRMLS_CC, E_WARNING, "Missing object traits reference index at position %d", pos - res);
				return -1;
			}

			traits = *found;
		} else {
			if (pfx & 4) {
				php_error_docref(NULL TSRMLS_CC, E_WARNING, "Can't decode externalizable object at position %d", pos);
				return -1;
			}

			members = pfx >> 4;
			if ((members < 0) || ((pos + members) > size)) {
				php_error_docref(NULL TSRMLS_CC, E_WARNING, "Invalid number of class members at position %d", pos - res);
				return -1;
			}

			res = amf3_decodeStr(&key, &keyLen, data + pos, size - pos, env); // class name
			if (res < 0) {
				php_error_docref(NULL TSRMLS_CC, E_WARNING, "Can't decode class name at position %d", pos);
				return -1;
			}
			if (members > 0 && !keyLen) {
				php_error_docref(NULL TSRMLS_CC, E_WARNING, "Empty class name at position %d", pos);
				return -1;
			}
			pos += res;

			traits = (amf3_traits_t *)ecalloc(1, sizeof(*traits));
			zend_hash_index_update(&env->traits, zend_hash_num_elements(&env->traits), &traits, sizeof(traits), NULL);

			if (keyLen) {
				int ret;
				// do not try to autoload class, autoloading based on user supplied data is a bad idea
				ret = zend_lookup_class_ex(key, keyLen, NULL, 0, &traits->ce TSRMLS_CC);
				if (ret == FAILURE) {
					php_error_docref(NULL TSRMLS_CC, E_WARNING, "Unable to find class at position %d", pos - res);
					return -1;
				}
			}

			traits->memberCount = members;
			if (members > 0) {
				char *tmpBuf;

				traits->members = (char **)ecalloc(traits->memberCount, sizeof(*traits->members));
				traits->memberLengths = (int *)ecalloc(traits->memberCount, sizeof(*traits->memberLengths));
				for (members = 0; members < traits->memberCount; members++) {
					res = amf3_decodeStr(&key, &keyLen, data + pos, size - pos, env); // member names
					if (res < 0) {
						return -1; // nested error
					}
					if (!keyLen || key[0] == 0) {
						php_error_docref(NULL TSRMLS_CC, E_WARNING, "Invalid member name at position %d", pos);
						return -1;
					}

					tmpBuf = (char *)emalloc(keyLen + 1);
					memcpy(tmpBuf, key, keyLen);
					tmpBuf[keyLen] = 0;
					if (!zend_hash_quick_exists(&(*traits->ce)->properties_info, tmpBuf, keyLen + 1, zend_get_hash_value(tmpBuf, keyLen + 1))) {
						efree(tmpBuf);
						php_error_docref(NULL TSRMLS_CC, E_WARNING, "Unknown member name at position %d", pos);
						return -1;
					}
					efree(tmpBuf);

					pos += res;

					traits->members[members] = estrndup(key, keyLen);
					traits->memberLengths[members] = keyLen + 1;
				}
			}

			traits->dynamic = pfx & 0x08;
		}

		amf3_initVal(val);
		if (traits->ce) {
			object_init_ex(*val, *traits->ce);
			if ((*traits->ce)->constructor) {
				zend_call_method_with_0_params(val, *traits->ce, &(*traits->ce)->constructor, NULL, NULL);
			}
		} else {
			object_init(*val);
		}
		amf3_putRef(&env->objs, *val);

		for (members = 0; members < traits->memberCount; members++) { // sealed members
			prop = 0;
			res = amf3_decodeVal(&prop, data, pos, size, env TSRMLS_CC);
			if (prop) {
				add_property_zval_ex(*val, traits->members[members], traits->memberLengths[members], prop TSRMLS_CC);
				Z_DELREF_P(prop);
			}
			if (res < 0) {
				return -1; // nested error
			}
			pos += res;
		}

		if (traits->dynamic) { // dynamic members
			char *tmpBuf;

			for ( ;; ) {
				res = amf3_decodeStr(&key, &keyLen, data + pos, size - pos, env);
				if (res < 0) {
					php_error_docref(NULL TSRMLS_CC, E_WARNING, "Can't decode dynamic member name at position %d", pos);
					return -1;
				}
				pos += res;
				if (!keyLen) {
					break;
				}
				if (key[0] == 0) {
					php_error_docref(NULL TSRMLS_CC, E_WARNING, "Invalid dynamic property name at position %d", pos - res);
					return -1;
				}

				if (traits->ce) {
					tmpBuf = (char *)emalloc(keyLen + 1);
					memcpy(tmpBuf, key, keyLen);
					tmpBuf[keyLen] = 0;
					if (zend_hash_quick_exists(&(*traits->ce)->properties_info, tmpBuf, keyLen + 1, zend_get_hash_value(tmpBuf, keyLen + 1))) {
						efree(tmpBuf);
						php_error_docref(NULL TSRMLS_CC, E_WARNING, "Invalid dynamic property name at position %d", pos - res);
						return -1;
					}
					efree(tmpBuf);
				}

				prop = 0;
				res = amf3_decodeVal(&prop, data, pos, size, env TSRMLS_CC);
				if (prop) { // need a trailing \0 in the key buffer to do a proper call to 'add_property_zval_ex'
					if (keyLen < sizeof(keyBuf)) {
						memcpy(keyBuf, key, keyLen);
						keyBuf[keyLen] = 0;
						add_property_zval_ex(*val, keyBuf, keyLen + 1, prop TSRMLS_CC);
					} else {
						char *tmpBuf = (char *)emalloc(keyLen + 1);
						memcpy(tmpBuf, key, keyLen);
						tmpBuf[keyLen] = 0;
						add_property_zval_ex(*val, tmpBuf, keyLen + 1, prop TSRMLS_CC);
						efree(tmpBuf);
					}
					Z_DELREF_P(prop);
				}
				if (res < 0) {
					return -1; // nested error
				}
				pos += res;
			}
		}
	}
	return pos - oldPos;
}

static int amf3_decodeXml(zval **val, const char *data, int pos, int size, amf3_env_t *env TSRMLS_DC) {
	int oldPos = pos;
	int pfx, res = amf3_decodeU29(&pfx, data + pos, size - pos);
	if (res < 0) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Can't decode XML prefix at position %d", pos);
		return -1;
	}
	pos += res;
	if (!(pfx & 1)) { // decode as a reference
		*val = amf3_getRef(&env->objs, pfx >> 1);
		if (!*val) {
			php_error_docref(NULL TSRMLS_CC, E_WARNING, "Missing XML reference index at position %d", pos - res);
			return -1;
		}
		Z_SET_ISREF_PP(val);
	} else {
		zval simplexml_load_string, *xml;

		pfx >>= 1;
		if ((pfx < 0) || ((pos + pfx) > size)) {
			php_error_docref(NULL TSRMLS_CC, E_WARNING, "Can't decode XML at position %d", pos);
			return -1;
		}

		INIT_ZVAL(simplexml_load_string);
		ZVAL_STRINGL(&simplexml_load_string, "simplexml_load_string", sizeof("simplexml_load_string") - 1, 0);

		amf3_initVal(val);

		MAKE_STD_ZVAL(xml);
		ZVAL_STRINGL(xml, data + pos, pfx, 0);
		if (call_user_function(EG(function_table), NULL, &simplexml_load_string, *val, 1, &xml TSRMLS_CC) == FAILURE ||
			(Z_TYPE_PP(val) == IS_BOOL && !Z_BVAL_PP(val))) {
			php_error_docref(NULL TSRMLS_CC, E_WARNING, "Can't load XML at position %d", pos);
			FREE_ZVAL(xml);
			return -1;
		}
		FREE_ZVAL(xml);

		amf3_putRef(&env->objs, *val);
		pos += pfx;
	}
	return pos - oldPos;
}

static int amf3_decodeVal(zval **val, const char *data, int pos, int size, amf3_env_t *env TSRMLS_DC) {
	if ((pos < 0) || (pos >= size)) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Can't decode type specifier at position %d", pos);
		return -1;
	}
	int oldPos = pos;
	amf3_type_t type = (unsigned int)data[pos++];
	switch (type) {
		case AMF3_UNDEFINED:
		case AMF3_NULL:
			amf3_initVal(val);
			ZVAL_NULL(*val);
			break;
		case AMF3_FALSE:
			amf3_initVal(val);
			ZVAL_FALSE(*val);
			break;
		case AMF3_TRUE:
			amf3_initVal(val);
			ZVAL_TRUE(*val);
			break;
		case AMF3_INTEGER: {
			int i;
			int res = amf3_decodeU29(&i, data + pos, size - pos);
			if (res < 0) {
				php_error_docref(NULL TSRMLS_CC, E_WARNING, "Can't decode integer at position %d", pos);
				return -1;
			}
			if (i & 0x10000000) {
				i |= ~0x1fffffff; // prolong sign bits if negative
			}
			amf3_initVal(val);
			ZVAL_LONG(*val, i);
			pos += res;
			break;
		}
		case AMF3_DOUBLE: {
			double d;
			int res = amf3_decodeDouble(&d, data + pos, size - pos);
			if (res < 0) {
				php_error_docref(NULL TSRMLS_CC, E_WARNING, "Can't decode double at position %d", pos);
				return -1;
			}
			amf3_initVal(val);
			ZVAL_DOUBLE(*val, d);
			pos += res;
			break;
		}
		case AMF3_STRING: {
			int pfx, res = amf3_decodeU29(&pfx, data + pos, size - pos);
			if (res < 0) {
				php_error_docref(NULL TSRMLS_CC, E_WARNING, "Can't decode string prefix at position %d", pos);
				return -1;
			}
			pos += res;
			if (!(pfx & 1)) { // decode as a reference
				*val = amf3_getRef(&env->strs, pfx >> 1);
				if (!*val) {
					php_error_docref(NULL TSRMLS_CC, E_WARNING, "Missing string reference index at position %d", pos - res);
					return -1;
				}
			} else {
				pfx >>= 1;
				if ((pfx < 0) || ((pos + pfx) > size)) {
					php_error_docref(NULL TSRMLS_CC, E_WARNING, "Invalid string length at position %d", pos - res);
					return -1;
				}
				amf3_initVal(val);
				ZVAL_STRINGL(*val, data + pos, pfx, 1);
				pos += pfx;
				if (pfx > 0) {
					amf3_putRef(&env->strs, *val); // empty string is never sent by reference
				}
			}
			break;
		}
		case AMF3_DATE: {
			int pfx, res = amf3_decodeU29(&pfx, data + pos, size - pos);
			if (res < 0) {
				php_error_docref(NULL TSRMLS_CC, E_WARNING, "Can't decode date prefix at position %d", pos);
				return -1;
			}
			pos += res;
			if (!(pfx & 1)) { // decode as a reference
				*val = amf3_getRef(&env->objs, pfx >> 1);
				if (!*val) {
					php_error_docref(NULL TSRMLS_CC, E_WARNING, "Missing date reference index at position %d", pos - res);
					return -1;
				}
				Z_SET_ISREF_PP(val);
			} else {
				double d;
				char buf[64];
				int bufLen;
				res = amf3_decodeDouble(&d, data + pos, size - pos);
				if (res < 0 || d < 0.0) {
					php_error_docref(NULL TSRMLS_CC, E_WARNING, "Can't decode date at position %d", pos);
					return -1;
				}

				amf3_initVal(val);
				bufLen = snprintf(buf, sizeof(buf), "%.0f", d/1000.0);
				php_date_instantiate(php_date_get_date_ce(), *val TSRMLS_CC);
				if (!php_date_initialize((php_date_obj *)zend_object_store_get_object(*val TSRMLS_CC), buf, bufLen, "U", NULL, 0 TSRMLS_CC)) {
					php_error_docref(NULL TSRMLS_CC, E_WARNING, "Can't initialize date at position %d", pos);
					return -1;
				}

				amf3_putRef(&env->objs, *val);
				pos += res;
			}
			break;
		}
		case AMF3_ARRAY: {
			int res = amf3_decodeArray(val, data, pos, size, env TSRMLS_CC);
			if (res < 0) {
				return -1; // nested error
			}
			pos += res;
			break;
		}
		case AMF3_OBJECT: {
			int res = amf3_decodeObject(val, data, pos, size, env TSRMLS_CC);
			if (res < 0) {
				return -1; // nested error
			}
			pos += res;
			break;
		}
		case AMF3_XMLDOC:
		case AMF3_XML: {
			int res = amf3_decodeXml(val, data, pos, size, env TSRMLS_CC);
			if (res < 0) {
				return -1; // nested error
			}
			pos += res;
			break;
		}
	  case AMF3_BYTEARRAY: {
			int pfx, res = amf3_decodeU29(&pfx, data + pos, size - pos);
			if (res < 0) {
				php_error_docref(NULL TSRMLS_CC, E_WARNING, "Can't decode byte array prefix at position %d", pos);
				return -1;
			}
			pos += res;
			if (!(pfx & 1)) { // decode as a reference
				*val = amf3_getRef(&env->objs, pfx >> 1);
				if (!*val) {
					php_error_docref(NULL TSRMLS_CC, E_WARNING, "Missing byte array reference index at position %d", pos - res);
					return -1;
				}
				Z_SET_ISREF_PP(val);
			} else {
				pfx >>= 1;
				if ((pfx < 0) || ((pos + pfx) > size)) {
					php_error_docref(NULL TSRMLS_CC, E_WARNING, "Invalid byte array length at position %d", pos - res);
					return -1;
				}
				amf3_initVal(val);
				ZVAL_STRINGL(*val, data + pos, pfx, 1);
				pos += pfx;
				amf3_putRef(&env->objs, *val);
			}
			break;
	  }
		default:
			php_error_docref(NULL TSRMLS_CC, E_WARNING, "Unsupported value type 0x%02X at position %d", type, pos - 1);
			return -1;
	}
	return pos - oldPos;
}

static void traits_ptr_dtor(void *ptr) {
	int i;
	amf3_traits_t *traits = *((amf3_traits_t **)ptr);
	if (traits->members) {
		for (i = 0; i < traits->memberCount; i++) {
			if (traits->members[i]) {
				efree(traits->members[i]);
			}
		}
		efree(traits->members);
	}
	if (traits->memberLengths) {
		efree(traits->memberLengths);
	}
	efree(traits);
}

static void amf3_initEnv(amf3_env_t *env, int dtor) {
	memset(env, 0, sizeof(*env));
	zend_hash_init(&env->strs, 10, NULL, dtor ? ZVAL_PTR_DTOR : NULL, 0);
	zend_hash_init(&env->objs, 10, NULL, dtor ? ZVAL_PTR_DTOR : NULL, 0);
	zend_hash_init(&env->traits, 10, NULL, dtor ? traits_ptr_dtor : NULL, 0);
}

static void amf3_destroyEnv(amf3_env_t *env) {
	zend_hash_destroy(&env->strs);
	zend_hash_destroy(&env->objs);
	zend_hash_destroy(&env->traits);
}


/* ============================================================================================================ */


/* {{{ proto string amf3_encode(mixed value)
   Encodes given value into AMF3 */
PHP_FUNCTION(amf3_encode) {
	zval *val;
	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "z", &val) == FAILURE) {
		RETURN_FALSE;
	}
	amf3_chunk_t *begin = amf3_initChunk();
	amf3_chunk_t *current = begin;
	amf3_env_t env;
	amf3_initEnv(&env, 0);
	int size = amf3_encodeVal(&current, val, &env TSRMLS_CC);
	amf3_destroyEnv(&env);
	char *buf = (char *)emalloc(size + 1);
	amf3_freeChunk(begin, buf);
	buf[size] = 0;
	RETURN_STRINGL(buf, size, 0);
}
/* }}} */

/* {{{ proto mixed amf3_decode(string data [, int &count])
   Decodes given AMF3 data to PHP type */
PHP_FUNCTION(amf3_decode) {
	char *data;
	int size;
	zval *count = 0;
	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s|z", &data, &size, &count) == FAILURE) {
		RETURN_FALSE;
	}
	amf3_env_t env;
	amf3_initEnv(&env, 1);
	int res = amf3_decodeVal(&return_value, data, 0, size, &env TSRMLS_CC);
	amf3_destroyEnv(&env);
	if (count) {
		ZVAL_LONG(count, res);
	}
	if (res < 0 && return_value) {
		zval_dtor(return_value);
		RETURN_NULL();
	}
}
/* }}} */
