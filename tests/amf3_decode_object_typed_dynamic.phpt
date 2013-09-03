--TEST--
AMF3 decoding - typed object with dynamic properties
--FILE--
<?php
class DTO {
	public $field = "value";
}

$count = 0;
$amf3 = pack("H*", "0905010a1b0744544f0b6669656c64060b76616c75650f64796e616d69630613736f6d657468696e67010a01060401");
$data = amf3_decode($amf3, $count);
var_dump($data);
var_dump($count);
?>
--EXPECT--
array(2) {
  [0]=>
  object(DTO)#1 (2) {
    ["field"]=>
    string(5) "value"
    ["dynamic"]=>
    string(9) "something"
  }
  [1]=>
  object(DTO)#2 (1) {
    ["field"]=>
    string(5) "value"
  }
}
int(47)
