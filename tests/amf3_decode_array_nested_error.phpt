--TEST--
AMF3 decoding - nested error decoding array
--INI--
display_errors=off
--FILE--
<?php
$amf3 = pack("H*", "0903010902");
$data = amf3_decode($amf3);
var_dump($data);
?>
--EXPECT--
NULL
