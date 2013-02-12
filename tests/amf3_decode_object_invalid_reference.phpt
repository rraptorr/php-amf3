--TEST--
AMF3 decoding - invalid object reference
--INI--
display_errors=off
--FILE--
<?php
var_dump(amf3_decode(pack("H*", "0a00")));
?>
--EXPECT--
NULL
