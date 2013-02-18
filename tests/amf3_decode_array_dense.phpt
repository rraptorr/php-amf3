--TEST--
AMF3 decoding - dense array
--FILE--
<?php
$count = 0;
$amf3 = pack("H*", "090b0104010402040306076162630607717765");
$data = amf3_decode($amf3, $count);
var_dump($data);
var_dump($count);
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
int(19)
