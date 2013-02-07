--TEST--
AMF3 decoding - nested error decoding array
--FILE--
<?php
ini_set("display_errors", "off");
$amf3 = hex2bin("0903010902");
$data = amf3_decode($amf3);
var_dump($data);
?>
--EXPECT--
NULL
