--TEST--
AMF3 decoding - invalid byte array reference
--INI--
display_errors=off
--FILE--
<?php
$count = 0;
$amf3 = pack("H*", "0905010c0d7177653132330c04");
$data = amf3_decode($amf3, $count);
var_dump($data);
var_dump($count);
?>
--EXPECT--
NULL
int(-1)
