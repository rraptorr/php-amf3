--TEST--
AMF3 decoding - undefined type
--FILE--
<?php
$count = 0;
var_dump(amf3_decode(pack("H*", "00"), $count));
var_dump($count);
?>
--EXPECT--
NULL
int(1)
