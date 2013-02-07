--TEST--
AMF3 encoding - integer
--FILE--
<?php
var_dump(bin2hex(amf3_encode(0)));
var_dump(bin2hex(amf3_encode(1)));
var_dump(bin2hex(amf3_encode(0x7F)));
var_dump(bin2hex(amf3_encode(0x80)));
var_dump(bin2hex(amf3_encode(0x3FFF)));
var_dump(bin2hex(amf3_encode(0x4000)));
var_dump(bin2hex(amf3_encode(0x1FFFFF)));
var_dump(bin2hex(amf3_encode(0x200000)));
var_dump(bin2hex(amf3_encode(0xFFFFFFF)));
var_dump(bin2hex(amf3_encode(0x10000000)));
var_dump(bin2hex(amf3_encode(-1)));
var_dump(bin2hex(amf3_encode(-0x10000000)));
var_dump(bin2hex(amf3_encode(-0x10000001)));
?>
--EXPECT--
string(4) "0400"
string(4) "0401"
string(4) "047f"
string(6) "048100"
string(6) "04ff7f"
string(8) "04818000"
string(8) "04ffff7f"
string(10) "0480c08000"
string(10) "04bfffffff"
string(18) "0541b0000000000000"
string(10) "04ffffffff"
string(10) "04c0808000"
string(18) "05c1b0000001000000"
