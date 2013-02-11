--TEST--
AMF3 decoding - invalid date
--FILE--
<?php
ini_set("display_errors", "off");
date_default_timezone_set('UTC');
$amf3 = pack("H*", "0903010801bff0000000000000");
$data = amf3_decode($amf3);
var_dump($data);
?>
--EXPECT--
NULL
