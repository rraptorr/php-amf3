--TEST--
AMF3 decoding - object traits
--FILE--
<?php
class DTO {
	public $field = "value";
}

$amf3 = hex2bin("0905010a130744544f0b6669656c64060b76616c75650a010604");
$data = amf3_decode($amf3);
var_dump($data);
?>
--EXPECT--
array(2) {
  [0]=>
  object(DTO)#1 (1) {
    ["field"]=>
    string(5) "value"
  }
  [1]=>
  object(DTO)#2 (1) {
    ["field"]=>
    string(5) "value"
  }
}
