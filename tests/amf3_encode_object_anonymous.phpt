--TEST--
AMF3 encoding - anonymous object
--FILE--
<?php
$data = (object)array("key" => "value", "another" => 123);
$amf3 = amf3_encode($data);
var_dump(bin2hex($amf3));
?>
--EXPECT--
string(50) "0a0b01076b6579060b76616c75650f616e6f74686572047b01"
