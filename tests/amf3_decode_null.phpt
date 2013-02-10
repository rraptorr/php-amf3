--TEST--
AMF3 decoding - NULL
--FILE--
<?php
var_dump(amf3_decode(pack("H*", "01")));
?>
--EXPECT--
NULL
