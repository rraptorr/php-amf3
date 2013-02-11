--TEST--
AMF3 decoding - undefined type
--FILE--
<?php
var_dump(amf3_decode(pack("H*", "00")));
?>
--EXPECT--
NULL
