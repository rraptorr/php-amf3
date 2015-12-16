--TEST--
AMF3 decoding - byte array reference
--FILE--
<?php
$count = 0;
$amf3 = pack("H*", "0905010c0d7177653132330c02");
$data = amf3_decode($amf3, $count);
var_dump($data);
var_dump($count);
?>
--EXPECT--
array(2) {
  [0]=>
  string(6) "qwe123"
  [1]=>
  string(6) "qwe123"
}
int(13)
