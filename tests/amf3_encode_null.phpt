--TEST--
AMF3 encoding - NULL
--FILE--
<?php
var_dump(bin2hex(amf3_encode(NULL)));
?>
--EXPECT--
string(2) "01"
