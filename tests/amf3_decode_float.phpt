--TEST--
AMF3 decoding - float
--FILE--
<?php
$count = 0;
var_dump(amf3_decode(pack("H*", "050000000000000000"), $count));
var_dump($count);

$count = 0;
var_dump(amf3_decode(pack("H*", "053ff0000000000000"), $count));
var_dump($count);

$count = 0;
var_dump(amf3_decode(pack("H*", "053ff1f7ced916872b"), $count));
var_dump($count);

$count = 0;
var_dump(amf3_decode(pack("H*", "05bff0000000000000"), $count));
var_dump($count);

$count = 0;
var_dump(amf3_decode(pack("H*", "0540f5d6d74dfce315"), $count));
var_dump($count);

$count = 0;
var_dump(amf3_decode(pack("H*", "0541f0000000000000"), $count));
var_dump($count);
?>
--EXPECT--
float(0)
int(9)
float(1)
int(9)
float(1.123)
int(9)
float(-1)
int(9)
float(89453.45654)
int(9)
float(4294967296)
int(9)
