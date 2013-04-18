--TEST--
AMF3 decoding - object traits
--FILE--
<?php
class DTO {
	public $field = "value";
}

$count = 0;
$amf3 = pack("H*", "0905010a130744544f0b6669656c64060b76616c75650a010606");
$data = amf3_decode($amf3, $count);
var_dump($data);
var_dump($count);
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
int(26)
