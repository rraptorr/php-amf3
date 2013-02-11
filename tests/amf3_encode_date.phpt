--TEST--
AMF3 encoding - date
--FILE--
<?php
date_default_timezone_set('UTC');
$date = new DateTime('2013-02-11 23:09:09');
$data = array(&$date, &$date);
$amf3 = amf3_encode($data);
var_dump(bin2hex($amf3));
?>
--EXPECT--
string(30) "09050108014273ccb84e2080000802"
