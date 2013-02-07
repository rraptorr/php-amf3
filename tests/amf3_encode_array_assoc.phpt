--TEST--
AMF3 encoding - associative array
--FILE--
<?php
$data = array("one" => 1, "two" => 2, "three" => 3, "four" => "abc", "five" => "qwe");
$amf3 = amf3_encode($data);
var_dump(bin2hex($amf3));
?>
--EXPECT--
string(86) "0901076f6e6504010774776f04020b7468726565040309666f757206076162630966697665060771776501"
