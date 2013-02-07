--TEST--
AMF3 encoding - empty array
--FILE--
<?php
$data = array();
$amf3 = amf3_encode($data);
var_dump(bin2hex($amf3));
?>
--EXPECT--
string(6) "090101"
