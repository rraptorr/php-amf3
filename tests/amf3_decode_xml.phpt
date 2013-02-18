--TEST--
AMF3 decoding - XML
--SKIPIF--
<?php if (!extension_loaded("simplexml")) echo "skip simplexml extension required"; ?>
--FILE--
<?php
$count = 0;
$amf3 = pack("H*", "0905010b5b3c3f786d6c2076657273696f6e3d22312e30223f3e0a3c783e3c6120617474723d2276616c222f3e3c2f783e0a0b02");
$data = amf3_decode($amf3, $count);
var_dump($data);
var_dump($count);
?>
--EXPECT--
array(2) {
  [0]=>
  &object(SimpleXMLElement)#1 (1) {
    ["a"]=>
    object(SimpleXMLElement)#2 (1) {
      ["@attributes"]=>
      array(1) {
        ["attr"]=>
        string(3) "val"
      }
    }
  }
  [1]=>
  &object(SimpleXMLElement)#1 (1) {
    ["a"]=>
    object(SimpleXMLElement)#2 (1) {
      ["@attributes"]=>
      array(1) {
        ["attr"]=>
        string(3) "val"
      }
    }
  }
}
int(52)
