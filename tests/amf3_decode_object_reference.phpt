--TEST--
AMF3 decoding - object reference
--FILE--
<?php
$count = 0;
$amf3 = pack("H*", "0905010a0b010b6669656c64060b76616c7565010a02");
$data = amf3_decode($amf3, $count);
var_dump($data);
var_dump($count);
?>
--EXPECT--
array(2) {
  [0]=>
  &object(stdClass)#1 (1) {
    ["field"]=>
    string(5) "value"
  }
  [1]=>
  &object(stdClass)#1 (1) {
    ["field"]=>
    string(5) "value"
  }
}
int(22)
