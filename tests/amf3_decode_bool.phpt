--TEST--
AMF3 decoding - boolean
--FILE--
<?php
var_dump(amf3_decode(pack("H*", "02")));
var_dump(amf3_decode(pack("H*", "03")));
?>
--EXPECT--
bool(false)
bool(true)
