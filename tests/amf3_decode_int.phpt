--TEST--
AMF3 decoding - integer
--FILE--
<?php
var_dump(amf3_decode(pack("H*", "0400")));
var_dump(amf3_decode(pack("H*", "0401")));
var_dump(amf3_decode(pack("H*", "047f")));
var_dump(amf3_decode(pack("H*", "048100")));
var_dump(amf3_decode(pack("H*", "04ff7f")));
var_dump(amf3_decode(pack("H*", "04818000")));
var_dump(amf3_decode(pack("H*", "04ffff7f")));
var_dump(amf3_decode(pack("H*", "0480c08000")));
var_dump(amf3_decode(pack("H*", "04bfffffff")));
var_dump(amf3_decode(pack("H*", "04ffffffff")));
var_dump(amf3_decode(pack("H*", "04c0808000")));
?>
--EXPECT--
int(0)
int(1)
int(127)
int(128)
int(16383)
int(16384)
int(2097151)
int(2097152)
int(268435455)
int(-1)
int(-268435456)
