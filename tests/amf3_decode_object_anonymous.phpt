--TEST--
AMF3 decoding - anonymous object
--FILE--
<?php
$amf3 = pack("H*", "0a0b01076b6579060b76616c75650f616e6f74686572047b01");
$data = amf3_decode($amf3);
var_dump($data);
?>
--EXPECT--
object(stdClass)#1 (2) {
  ["key"]=>
  string(5) "value"
  ["another"]=>
  int(123)
}
