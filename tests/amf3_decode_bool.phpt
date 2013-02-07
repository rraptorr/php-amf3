--TEST--
AMF3 decoding - boolean
--FILE--
<?php
var_dump(amf3_decode(hex2bin("02")));
var_dump(amf3_decode(hex2bin("03")));
?>
--EXPECT--
bool(false)
bool(true)
