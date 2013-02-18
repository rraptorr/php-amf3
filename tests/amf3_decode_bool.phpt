--TEST--
AMF3 decoding - boolean
--FILE--
<?php
$count = 0;
var_dump(amf3_decode(pack("H*", "02"), $count));
var_dump($count);

$count = 0;
var_dump(amf3_decode(pack("H*", "03"), $count));
var_dump($count);
?>
--EXPECT--
bool(false)
int(1)
bool(true)
int(1)
