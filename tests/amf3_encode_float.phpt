--TEST--
AMF3 encoding - float
--FILE--
<?php
var_dump(bin2hex(amf3_encode(0.0)));
var_dump(bin2hex(amf3_encode(1.0)));
var_dump(bin2hex(amf3_encode(1.123)));
var_dump(bin2hex(amf3_encode(-1.0)));
var_dump(bin2hex(amf3_encode(89453.45654)));
var_dump(bin2hex(amf3_encode(4294967296)));
?>
--EXPECT--
string(18) "050000000000000000"
string(18) "053ff0000000000000"
string(18) "053ff1f7ced916872b"
string(18) "05bff0000000000000"
string(18) "0540f5d6d74dfce315"
string(18) "0541f0000000000000"
