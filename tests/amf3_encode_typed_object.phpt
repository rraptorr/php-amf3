--TEST--
AMF3 encoding - typed object
--FILE--
<?php
class DTO {
	public $field = "value";
	public $id = 678;
}

$data = new DTO();
$amf3 = amf3_encode($data);
var_dump(bin2hex($amf3));
?>
--EXPECT--
string(50) "0a230744544f0b6669656c64056964060b76616c7565048526"
