--TEST--
Test root on scalar true
--SKIPIF--
<?php if (!extension_loaded("jsonpath")) print "skip"; ?>
<?php if (version_compare(PHP_VERSION, '8.0.0', '<')) print "skip"; ?>
--FILE--
<?php

$data = true;

$jsonPath = new JsonPath();
$result = $jsonPath->find($data, "$");

var_dump($result);
?>
--EXPECTF--
Fatal error: Uncaught TypeError: JsonPath::find(): Argument #1 ($data) must be of type array, bool given in %s004_php8.php:%d
Stack trace:
#0 %s004_php8.php(%d): JsonPath->find(true, '$')
#1 {main}
  thrown in %s004_php8.php on line %d
