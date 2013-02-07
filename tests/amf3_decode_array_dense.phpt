--TEST--
AMF3 decoding - dense array
--FILE--
<?php
$amf3 = hex2bin("090b0104010402040306076162630607717765");
$data = amf3_decode($amf3);
var_dump($data);
?>
--EXPECT--
array(5) {
  [0]=>
  int(1)
  [1]=>
  int(2)
  [2]=>
  int(3)
  [3]=>
  string(3) "abc"
  [4]=>
  string(3) "qwe"
}
