--TEST--
AMF3 encoding - XML
--SKIPIF--
<?php if (!extension_loaded("simplexml")) echo "skip simplexml extension required"; ?>
--FILE--
<?php
$xml = simplexml_load_string("<x><a attr='val'/></x>");
$data = array($xml, $xml);
$amf3 = amf3_encode($data);
var_dump(bin2hex($amf3));
?>
--EXPECT--
string(104) "0905010b5b3c3f786d6c2076657273696f6e3d22312e30223f3e0a3c783e3c6120617474723d2276616c222f3e3c2f783e0a0b02"
