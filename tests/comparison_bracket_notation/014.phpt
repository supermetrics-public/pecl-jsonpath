--TEST--
Test bracket notation with number on string
--SKIPIF--
<?php if (!extension_loaded("jsonpath")) print "skip"; ?>
--FILE--
<?php

$data = "Hello World";

$jsonPath = new JsonPath();
$result = $jsonPath->find($data, '$[0]');

echo "Assertion 1\n";
var_dump($result);
?>
--EXPECTF--
Fatal error: Uncaught TypeError: JsonPath::find(): Argument #1 ($data) must be of type array, string given in %s014.php:%d
Stack trace:
#0 %s014.php(%d): JsonPath->find('Hello World', '$[0]')
#1 {main}
  thrown in %s014.php on line %d
