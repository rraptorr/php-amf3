--TEST--
AMF3 decoding - invalid string reference
--INI--
display_errors=off
--FILE--
<?php
$count = 0;
var_dump(amf3_decode(pack("H*", "0600"), $count));
var_dump($count);
?>
--EXPECT--
NULL
int(-1)
