--TEST--
AMF3 encoding/decoding - boolean type
--FILE--
<?php
var_dump(amf3_decode(amf3_encode(true)));
var_dump(amf3_decode(amf3_encode(false)));
?>
--EXPECT--
bool(true)
bool(false)
