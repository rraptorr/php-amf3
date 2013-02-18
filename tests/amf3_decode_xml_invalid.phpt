--TEST--
AMF3 decoding - invalid XML
--SKIPIF--
<?php if (!extension_loaded("simplexml")) echo "skip simplexml extension required"; ?>
--INI--
display_errors=off
--FILE--
<?php
$count = 0;
$amf3 = pack("H*", "0905010b573c3f786d6c2076657273696f6e3d22312e30223f3e0a3c783e3c6120617474723d2276616c222f3e3c2f780b02");
$data = amf3_decode($amf3, $count);
var_dump($data);
var_dump($count);
?>
--EXPECT--
NULL
int(-1)
