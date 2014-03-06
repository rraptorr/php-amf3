--TEST--
AMF3 decoding - invalid byte array
--INI--
display_errors=off
--FILE--
<?php
$count = 0;
$amf3 = pack("H*", "0c0d000010203");
$data = amf3_decode($amf3, $count);
var_dump($data);
var_dump($count);
?>
--EXPECT--
NULL
int(-1)
