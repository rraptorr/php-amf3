--TEST--
AMF3 encoding/decoding - float type
--FILE--
<?php
var_dump(amf3_decode(amf3_encode(0.0)));
var_dump(amf3_decode(amf3_encode(1.0)));
var_dump(amf3_decode(amf3_encode(1.123)));
var_dump(amf3_decode(amf3_encode(-1.0)));
var_dump(amf3_decode(amf3_encode(89453.45654)));
var_dump(amf3_decode(amf3_encode(4294967296)));
?>
--EXPECT--
float(0)
float(1)
float(1.123)
float(-1)
float(89453.45654)
float(4294967296)
