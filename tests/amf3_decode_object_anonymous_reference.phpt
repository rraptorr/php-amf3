--TEST--
AMF3 decoding - reference to anonymous object traits
--FILE--
<?php
$amf3 = pack("H*", "0a0b010970726f700a01036104010101");
$data = amf3_decode($amf3);
var_dump($data);
?>
--EXPECT--
object(stdClass)#1 (1) {
  ["prop"]=>
  object(stdClass)#2 (1) {
    ["a"]=>
    int(1)
  }
}
