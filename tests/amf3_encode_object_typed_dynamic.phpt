--TEST--
AMF3 encoding - typed object with dynamic properties
--FILE--
<?php
class DTO {
	public $field = "value";
}

$o1 = new DTO();
$o1->dynamic = "something";
$o2 = new DTO();
$data = array($o1, $o2);
$amf3 = amf3_encode($data);
var_dump(bin2hex($amf3));
?>
--EXPECT--
string(94) "0905010a1b0744544f0b6669656c64060b76616c75650f64796e616d69630613736f6d657468696e67010a01060401"
