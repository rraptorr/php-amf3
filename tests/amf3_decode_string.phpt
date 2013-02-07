--TEST--
AMF3 decoding - string
--FILE--
<?php
var_dump(amf3_decode(hex2bin("0601")));
var_dump(amf3_decode(hex2bin("060f6f6e650a74776f")));
var_dump(amf3_decode(hex2bin("060f6f6e650074776f")));
var_dump(amf3_decode(hex2bin("062f6f6e652074776f207468726565206669766520666f7572")));
var_dump(amf3_decode(hex2bin("06357a61c5bcc3b3c582c4872067c499c59b6cc485206a61c5bac584")));
?>
--EXPECT--
string(0) ""
string(7) "one
two"
string(7) "one two"
string(23) "one two three five four"
string(26) "zażółć gęślą jaźń"
