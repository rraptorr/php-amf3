--TEST--
AMF3 decoding - externalizable object
--INI--
display_errors=off
--FILE--
<?php
var_dump(amf3_decode(pack("H*", "0a07")));
?>
--EXPECT--
NULL
