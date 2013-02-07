--TEST--
AMF3 decoding - float
--FILE--
<?php
var_dump(amf3_decode(hex2bin("050000000000000000")));
var_dump(amf3_decode(hex2bin("053ff0000000000000")));
var_dump(amf3_decode(hex2bin("053ff1f7ced916872b")));
var_dump(amf3_decode(hex2bin("05bff0000000000000")));
var_dump(amf3_decode(hex2bin("0540f5d6d74dfce315")));
var_dump(amf3_decode(hex2bin("0541f0000000000000")));
?>
--EXPECT--
float(0)
float(1)
float(1.123)
float(-1)
float(89453.45654)
float(4294967296)
