--TEST--
AMF3 decoding - object autoloading
--INI--
display_errors=off
--FILE--
<?php
function autoloader($class) {
	echo "autoloader called\n";

	class DTO {
		private $private = "private value";
		protected $protected = "protected value";
		public $_underscore = "underscore value";
		public $field;
		public $id;
	}
}

spl_autoload_register('autoloader');

$amf3 = pack("H*", "0a230744544f0b6669656c64056964060b76616c7565048526");
$data = amf3_decode($amf3);
var_dump($data);
?>
--EXPECT--
NULL
