--TEST--
AMF3 decoding - invalid string reference
--FILE--
<?php
ini_set("display_errors", "off");
var_dump(amf3_decode(pack("H*", "0600")));
?>
--EXPECT--
NULL
