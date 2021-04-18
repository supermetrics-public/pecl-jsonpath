--TEST--
Test root on scalar
--SKIPIF--
<?php if (!extension_loaded("jsonpath")) print "skip"; ?>
<?php if (version_compare(PHP_VERSION, '8.0.0', '>=')) print "skip"; ?>
--FILE--
<?php

$data = 42;

$jsonPath = new JsonPath();
$result = $jsonPath->find($data, "$");

echo "Assertion 1\n";
var_dump($result);
?>
--EXPECTF--
Warning: JsonPath::find() expects parameter 1 to be array, int given in %s002_php7.php on line %d
Assertion 1
NULL
