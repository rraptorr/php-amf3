--TEST--
AMF3 decoding - typed object with broken traits
--FILE--
<?php

class DTO {

}

$amf3 = pack("H*", "0a230744544f0b66");
$data = amf3_decode($amf3);
var_dump($data);
?>
--EXPECT--
NULL
