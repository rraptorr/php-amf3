--TEST--
AMF3 encoding/decoding - anonymous object
--FILE--
<?php
var_dump(amf3_decode(amf3_encode((object)array("key" => "value", "another" => 123))));
?>
--EXPECT--
object(stdClass)#1 (2) {
  ["key"]=>
  string(5) "value"
  ["another"]=>
  int(123)
}
