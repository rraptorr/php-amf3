--TEST--
AMF3 decoding - typed object with invalid dynamic property name
--INI--
display_errors=off
--FILE--
<?php
class DTO {
	public $field = "value";
	public $dynamic = "value";
}

$count = 0;
$amf3 = pack("H*", "0a1b0744544f0b6669656c64060b76616c75650f64796e616d6963060401");
$data = amf3_decode($amf3, $count);
var_dump($data);
var_dump($count);
?>
--EXPECT--
NULL
int(-1)
