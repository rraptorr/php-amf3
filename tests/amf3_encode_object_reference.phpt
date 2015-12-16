--TEST--
AMF3 encoding - object reference
--FILE--
<?php
$dto = (object)array("field" => "value");
$data = array($dto, $dto);
$amf3 = amf3_encode($data);
var_dump(bin2hex($amf3));
?>
--EXPECT--
string(44) "0905010a0b010b6669656c64060b76616c7565010a02"
