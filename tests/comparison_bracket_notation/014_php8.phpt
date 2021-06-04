--TEST--
Test bracket notation with number on string
--SKIPIF--
<?php if (!extension_loaded("jsonpath")) print "skip"; ?>
<?php if (version_compare(PHP_VERSION, '8.0.0', '<')) print "skip"; ?>
--FILE--
<?php

$data = "Hello World";

$jsonPath = new JsonPath();
$result = $jsonPath->find($data, '$[0]');

var_dump($result);
?>
--EXPECTF--
Fatal error: Uncaught TypeError: JsonPath::find(): Argument #1 ($data) must be of type array, string given in %s014_php8.php:%d
Stack trace:
#0 %s014_php8.php(%d): JsonPath->find('Hello World', '$[0]')
#1 {main}
  thrown in %s014_php8.php on line %d
