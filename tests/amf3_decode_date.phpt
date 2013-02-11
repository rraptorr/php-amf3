--TEST--
AMF3 decoding - date
--FILE--
<?php
date_default_timezone_set('UTC');
$amf3 = pack("H*", "09050108014273ccb84e2080000802");
$data = amf3_decode($amf3);
var_dump($data);
?>
--EXPECT--
array(2) {
  [0]=>
  &object(DateTime)#1 (3) {
    ["date"]=>
    string(19) "2013-02-11 23:09:09"
    ["timezone_type"]=>
    int(1)
    ["timezone"]=>
    string(6) "+00:00"
  }
  [1]=>
  &object(DateTime)#1 (3) {
    ["date"]=>
    string(19) "2013-02-11 23:09:09"
    ["timezone_type"]=>
    int(1)
    ["timezone"]=>
    string(6) "+00:00"
  }
}
