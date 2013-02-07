--TEST--
AMF3 encoding - array reference
--FILE--
<?php
$arr = array(1, 2, 3);
$data = array(&$arr, &$arr);
$amf3 = amf3_encode($data);
var_dump(bin2hex($amf3));
?>
--EXPECT--
string(28) "0905010907010401040204030902"
