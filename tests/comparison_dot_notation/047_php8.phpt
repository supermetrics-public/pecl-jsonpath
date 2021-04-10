--TEST--
Test dot notation with wildcard after recursive descent on scalar
--SKIPIF--
<?php if (!extension_loaded("jsonpath")) print "skip"; ?>
<?php if (version_compare(PHP_VERSION, '8.0.0', '<')) print "skip"; ?>
--FILE--
<?php

$data = 42;

$jsonPath = new JsonPath();
$result = $jsonPath->find($data, "$..*");

echo "Assertion 1\n";
var_dump($result);
?>
--EXPECTF--
Fatal error: Uncaught TypeError: JsonPath::find(): Argument #1 ($data) must be of type array, int given in %s047_php8.php:%d
Stack trace:
#0 %s047_php8.php(%d): JsonPath->find(42, '$..*')
#1 {main}
  thrown in %s047_php8.php on line %d
