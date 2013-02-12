--TEST--
AMF3 decoding - invalid array reference
--INI--
display_errors=off
--FILE--
<?php
var_dump(amf3_decode(pack("H*", "0900")));
?>
--EXPECT--
NULL
