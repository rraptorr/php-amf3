--TEST--
AMF3 decoding - empty array
--FILE--
<?php
$count = 0;
$amf3 = pack("H*", "090101");
$data = amf3_decode($amf3, $count);
var_dump($data);
var_dump($count)
?>
--EXPECT--
array(0) {
}
int(3)
