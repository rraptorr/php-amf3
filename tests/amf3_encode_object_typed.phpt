--TEST--
AMF3 encoding - typed object
--FILE--
<?php
class DTO {
	private $private = "private value";
	protected $protected = "protected value";
	public $field = "value";
	public $id = 678;
}

$data = new DTO();
$amf3 = amf3_encode($data);
var_dump(bin2hex($amf3));
?>
--EXPECT--
string(52) "0a2b0744544f0b6669656c64056964060b76616c756504852601"
