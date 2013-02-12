--TEST--
AMF3 decoding - invalid string reference
--INI--
display_errors=off
--FILE--
<?php
var_dump(amf3_decode(pack("H*", "0600")));
?>
--EXPECT--
NULL
