--TEST--
AMF3 decoding - byte array
--FILE--
<?php
$count = 0;
$amf3 = pack("H*", "0c0d000102030405");
$data = amf3_decode($amf3, $count);
var_dump(bin2hex($data));
var_dump($count);
?>
--EXPECT--
string(12) "000102030405"
int(8)
