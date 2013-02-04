--TEST--
AMF3 encoding/decoding - mixed array
--FILE--
<?php
$arr = array(1, 2, 3, 'four' => 4, 'five' => 5);
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
  ["four"]=>
  int(4)
  ["five"]=>
  int(5)
}
