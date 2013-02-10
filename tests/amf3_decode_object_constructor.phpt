--TEST--
AMF3 decoding - typed object with constructor
--FILE--
<?php
class DTO {
	private $private = "private value";
	protected $protected = "protected value";
	public $_underscore = "underscore value";
	public $field;
	public $id;

	public function __construct() {
		$this->extra = 123;
	}
}

$amf3 = pack("H*", "0a230744544f0b6669656c64056964060b76616c7565048526");
$data = amf3_decode($amf3);
var_dump($data);
?>
--EXPECT--
object(DTO)#1 (6) {
  ["private":"DTO":private]=>
  string(13) "private value"
  ["protected":protected]=>
  string(15) "protected value"
  ["_underscore"]=>
  string(16) "underscore value"
  ["field"]=>
  string(5) "value"
  ["id"]=>
  int(678)
  ["extra"]=>
  int(123)
}
