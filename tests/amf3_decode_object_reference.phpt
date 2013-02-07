--TEST--
AMF3 decoding - object reference
--FILE--
<?php
$amf3 = hex2bin("0905010a0b010b6669656c64060b76616c7565010a02");
$data = amf3_decode($amf3);
var_dump($data);
?>
--EXPECT--
array(2) {
  [0]=>
  &object(stdClass)#1 (1) {
    ["field"]=>
    string(5) "value"
  }
  [1]=>
  &object(stdClass)#1 (1) {
    ["field"]=>
    string(5) "value"
  }
}
