--TEST--
AMF3 decoding - NULL
--FILE--
<?php
$count = 0;
var_dump(amf3_decode(pack("H*", "01"), $count));
var_dump($count)
?>
--EXPECT--
NULL
int(1)
