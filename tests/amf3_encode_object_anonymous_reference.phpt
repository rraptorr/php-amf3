--TEST--
AMF3 encoding - reference to anonymous object traits
--FILE--
<?php
$obj = array();
$obj['prop'] = (object)array("a" => 1);
$obj = (object)$obj;
var_dump(bin2hex(amf3_encode($obj)));
?>
--EXPECT--
string(32) "0a0b010970726f700a01036104010101"
