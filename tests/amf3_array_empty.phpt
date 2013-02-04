--TEST--
AMF3 encoding/decoding - empty array
--FILE--
<?php
var_dump(amf3_decode(amf3_encode(array())));
?>
--EXPECT--
array(0) {
}
