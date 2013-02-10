--TEST--
AMF3 decoding - associative array
--FILE--
<?php
$amf3 = pack("H*", "0901076f6e6504010774776f04020b7468726565040309666f757206076162630966697665060771776501");
$data = amf3_decode($amf3);
var_dump($data);
?>
--EXPECT--
array(5) {
  ["one"]=>
  int(1)
  ["two"]=>
  int(2)
  ["three"]=>
  int(3)
  ["four"]=>
  string(3) "abc"
  ["five"]=>
  string(3) "qwe"
}
