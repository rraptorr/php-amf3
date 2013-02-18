--TEST--
AMF3 decoding - typed object with broken traits
--FILE--
<?php

class DTO {

}

$count = 0;
$amf3 = pack("H*", "0a230744544f0b66");
$data = amf3_decode($amf3, $count);
var_dump($data);
var_dump($count);
?>
--EXPECT--
NULL
int(-1)
