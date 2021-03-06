--TEST--
AMF3 decoding - string
--FILE--
<?php
$count = 0;
var_dump(amf3_decode(pack("H*", "0601"), $count));
var_dump($count);

$count = 0;
var_dump(amf3_decode(pack("H*", "060f6f6e650a74776f"), $count));
var_dump($count);

$count = 0;
var_dump(amf3_decode(pack("H*", "060f6f6e650074776f"), $count));
var_dump($count);

$count = 0;
var_dump(amf3_decode(pack("H*", "062f6f6e652074776f207468726565206669766520666f7572"), $count));
var_dump($count);

$count = 0;
var_dump(amf3_decode(pack("H*", "06357a61c5bcc3b3c582c4872067c499c59b6cc485206a61c5bac584"), $count));
var_dump($count);
?>
--EXPECT--
string(0) ""
int(2)
string(7) "one
two"
int(9)
string(7) "one two"
int(9)
string(23) "one two three five four"
int(25)
string(26) "zażółć gęślą jaźń"
int(28)
