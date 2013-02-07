--TEST--
AMF3 encoding - object traits
--FILE--
<?php
class DTO {
	public $field = "value";
}

$data = array(new DTO(), new DTO());
$amf3 = amf3_encode($data);
var_dump(bin2hex($amf3));
?>
--EXPECT--
string(52) "0905010a130744544f0b6669656c64060b76616c75650a010604"
