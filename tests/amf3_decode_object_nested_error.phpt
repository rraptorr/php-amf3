--TEST--
AMF3 decoding - nested error decoding object
--INI--
display_errors=off
--FILE--
<?php
$amf3 = pack("H*", "0a0b01076172720902");
$data = amf3_decode($amf3);
var_dump($data);
?>
--EXPECT--
NULL
