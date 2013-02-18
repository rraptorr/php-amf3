--TEST--
AMF3 decoding - nested error decoding array
--INI--
display_errors=off
--FILE--
<?php
$count = 0;
$amf3 = pack("H*", "0903010902");
$data = amf3_decode($amf3, $count);
var_dump($data);
var_dump($count);
?>
--EXPECT--
NULL
int(-1)
