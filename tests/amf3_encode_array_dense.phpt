--TEST--
AMF3 encoding - dense array
--FILE--
<?php
$data = array(1, 2, 3, "abc", "qwe");
$amf3 = amf3_encode($data);
var_dump(bin2hex($amf3));
?>
--EXPECT--
string(38) "090b0104010402040306076162630607717765"
