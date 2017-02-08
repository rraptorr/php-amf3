--TEST--
AMF3 decoding - reference to anonymous object field name
--FILE--
<?php
$count = 0;
$amf3 = pack("H*", "0905010a0b010361060362010a0100060201");
$data = amf3_decode($amf3, $count);
var_dump($data);
var_dump($count);
?>
--EXPECT--
array(2) {
  [0]=>
  object(stdClass)#1 (1) {
    ["a"]=>
    string(1) "b"
  }
  [1]=>
  object(stdClass)#2 (1) {
    ["a"]=>
    string(1) "b"
  }
}
int(18)
