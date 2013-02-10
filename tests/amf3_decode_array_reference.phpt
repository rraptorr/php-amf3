--TEST--
AMF3 decoding - array reference
--FILE--
<?php
$amf3 = pack("H*", "0905010907010401040204030902");
$data = amf3_decode($amf3);
var_dump($data);
?>
--EXPECT--
array(2) {
  [0]=>
  &array(3) {
    [0]=>
    int(1)
    [1]=>
    int(2)
    [2]=>
    int(3)
  }
  [1]=>
  &array(3) {
    [0]=>
    int(1)
    [1]=>
    int(2)
    [2]=>
    int(3)
  }
}
