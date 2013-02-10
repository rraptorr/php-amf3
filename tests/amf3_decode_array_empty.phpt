--TEST--
AMF3 decoding - empty array
--FILE--
<?php
$amf3 = pack("H*", "090101");
$data = amf3_decode($amf3);
var_dump($data);
?>
--EXPECT--
array(0) {
}
