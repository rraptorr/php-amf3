--TEST--
AMF3 decoding - invalid date
--INI--
display_errors=off
date.timezone=UTC
--FILE--
<?php
$count = 0;
$amf3 = pack("H*", "0903010801bff0000000000000");
$data = amf3_decode($amf3, $count);
var_dump($data);
var_dump($count);
?>
--EXPECT--
NULL
int(-1)
