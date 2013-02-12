--TEST--
AMF3 decoding - invalid date
--INI--
display_errors=off
date.timezone=UTC
--FILE--
<?php
$amf3 = pack("H*", "0903010801bff0000000000000");
$data = amf3_decode($amf3);
var_dump($data);
?>
--EXPECT--
NULL
