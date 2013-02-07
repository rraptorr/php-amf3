--TEST--
AMF3 decoding - NULL
--FILE--
<?php
var_dump(amf3_decode(hex2bin("01")));
?>
--EXPECT--
NULL
