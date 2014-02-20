--TEST--
AMF3 encoding - anonymous object with int property
--FILE--
<?php
$data = (object)array(123 => 123);
$amf3 = amf3_encode($data);
var_dump(bin2hex($amf3));
?>
--EXPECT--
string(20) "0a0b0107313233047b01"
