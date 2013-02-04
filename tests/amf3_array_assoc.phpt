--TEST--
AMF3 encoding/decoding - associative array
--FILE--
<?php
$arr = array('one' => 1, 'two' => 2, 'three' => 3, 'four' => 4, 'five' => 5);
var_dump(amf3_decode(amf3_encode($arr)));
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
  int(4)
  ["five"]=>
  int(5)
}
