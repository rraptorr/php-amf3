--TEST--
AMF3 encoding - string
--FILE--
<?php
var_dump(bin2hex(amf3_encode("")));
var_dump(bin2hex(amf3_encode("one\ntwo")));
var_dump(bin2hex(amf3_encode("one\0two")));
var_dump(bin2hex(amf3_encode("one two three five four")));
var_dump(bin2hex(amf3_encode("zażółć gęślą jaźń")));
?>
--EXPECT--
string(4) "0601"
string(18) "060f6f6e650a74776f"
string(18) "060f6f6e650074776f"
string(50) "062f6f6e652074776f207468726565206669766520666f7572"
string(56) "06357a61c5bcc3b3c582c4872067c499c59b6cc485206a61c5bac584"
