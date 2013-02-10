--TEST--
AMF3 decoding - externalizable object
--FILE--
<?php
ini_set("display_errors", "off");
var_dump(amf3_decode(hex2bin("0a07")));
?>
--EXPECT--
NULL
