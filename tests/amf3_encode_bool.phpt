--TEST--
AMF3 encoding - boolean
--FILE--
<?php
var_dump(bin2hex(amf3_encode(false)));
var_dump(bin2hex(amf3_encode(true)));
?>
--EXPECT--
string(2) "02"
string(2) "03"
