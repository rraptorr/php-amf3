--TEST--
AMF3 encoding/decoding - dense array
--FILE--
<?php
$arr = array(1, 2, 3, 4, 5);
var_dump(amf3_decode(amf3_encode($arr)));
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
  int(4)
  [4]=>
  int(5)
}
