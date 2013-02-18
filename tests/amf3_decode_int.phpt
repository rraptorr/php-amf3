--TEST--
AMF3 decoding - integer
--FILE--
<?php
$count = 0;
var_dump(amf3_decode(pack("H*", "0400"), $count));
var_dump($count);

$count = 0;
var_dump(amf3_decode(pack("H*", "0401"), $count));
var_dump($count);

$count = 0;
var_dump(amf3_decode(pack("H*", "047f"), $count));
var_dump($count);

$count = 0;
var_dump(amf3_decode(pack("H*", "048100"), $count));
var_dump($count);

$count = 0;
var_dump(amf3_decode(pack("H*", "04ff7f"), $count));
var_dump($count);

$count = 0;
var_dump(amf3_decode(pack("H*", "04818000"), $count));
var_dump($count);

$count = 0;
var_dump(amf3_decode(pack("H*", "04ffff7f"), $count));
var_dump($count);

$count = 0;
var_dump(amf3_decode(pack("H*", "0480c08000"), $count));
var_dump($count);

$count = 0;
var_dump(amf3_decode(pack("H*", "04bfffffff"), $count));
var_dump($count);

$count = 0;
var_dump(amf3_decode(pack("H*", "04ffffffff"), $count));
var_dump($count);

$count = 0;
var_dump(amf3_decode(pack("H*", "04c0808000"), $count));
var_dump($count);
?>
--EXPECT--
int(0)
int(2)
int(1)
int(2)
int(127)
int(2)
int(128)
int(3)
int(16383)
int(3)
int(16384)
int(4)
int(2097151)
int(4)
int(2097152)
int(5)
int(268435455)
int(5)
int(-1)
int(5)
int(-268435456)
int(5)
