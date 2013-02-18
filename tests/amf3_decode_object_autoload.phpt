--TEST--
AMF3 decoding - object autoloading
--DESCRIPTION--
This test ensures that autoloader does _not_ get called when deserializing typed objects.
Trying to load arbitrary class based on user input is generally a bad idea.
--INI--
display_errors=off
--FILE--
<?php
function autoloader($class) {
	echo "autoloader called\n";

	class DTO {
		private $private = "private value";
		protected $protected = "protected value";
		public $field;
		public $id;
	}
}

spl_autoload_register('autoloader');

$count = 0;
$amf3 = pack("H*", "0a230744544f0b6669656c64056964060b76616c7565048526");
$data = amf3_decode($amf3, $count);
var_dump($data);
var_dump($count)
?>
--EXPECT--
NULL
int(-1)
