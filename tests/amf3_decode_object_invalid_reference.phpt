--TEST--
AMF3 decoding - invalid object reference
--FILE--
<?php
ini_set("display_errors", "off");
var_dump(amf3_decode(hex2bin("0a00")));
?>
--EXPECT--
NULL