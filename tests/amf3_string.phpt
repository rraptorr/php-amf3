--TEST--
AMF3 encoding/decoding - string type
--FILE--
<?php
var_dump(amf3_decode(amf3_encode('')));
var_dump(amf3_decode(amf3_encode("one\ntwo")));
var_dump(amf3_decode(amf3_encode("one\0two")));
var_dump(amf3_decode(amf3_encode('one two three five four')));
var_dump(amf3_decode(amf3_encode('zażółć gęślą jaźń')));
?>
--EXPECT--
string(0) ""
string(7) "one
two"
string(7) "one two"
string(23) "one two three five four"
string(26) "zażółć gęślą jaźń"
