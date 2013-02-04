--TEST--
AMF3 encoding/decoding - NULL type
--FILE--
<?php
var_dump(amf3_decode(amf3_encode(NULL)));
?>
--EXPECT--
NULL
