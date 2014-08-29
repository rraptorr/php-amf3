--TEST--
AMF3 decoding - date
--INI--
date.timezone=UTC
--FILE--
<?php
$count = 0;
$amf3 = pack("H*", "09050108014273ccb84e2080000802");
$data = amf3_decode($amf3, $count);
var_dump($data);
var_dump($count);
?>
--EXPECTF--
array(2) {
  [0]=>
  &object(DateTime)#1 (3) {
    ["date"]=>
    string(%d) "2013-02-11 23:09:09%S"
    ["timezone_type"]=>
    int(1)
    ["timezone"]=>
    string(6) "+00:00"
  }
  [1]=>
  &object(DateTime)#1 (3) {
    ["date"]=>
    string(%d) "2013-02-11 23:09:09%S"
    ["timezone_type"]=>
    int(1)
    ["timezone"]=>
    string(6) "+00:00"
  }
}
int(15)
