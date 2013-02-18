--TEST--
AMF3 decoding - invalid object reference
--INI--
display_errors=off
--FILE--
<?php
$count = 0;
var_dump(amf3_decode(pack("H*", "0a00"), $count));
var_dump($count);
?>
--EXPECT--
NULL
int(-1)
