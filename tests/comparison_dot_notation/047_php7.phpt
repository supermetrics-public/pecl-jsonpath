--TEST--
Test dot notation with wildcard after recursive descent on scalar
--SKIPIF--
<?php if (!extension_loaded("jsonpath")) print "skip"; ?>
<?php if (version_compare(PHP_VERSION, '8.0.0', '>=')) print "skip"; ?>
--FILE--
<?php

$data = 42;

$jsonPath = new \JsonPath\JsonPath();
$result = $jsonPath->find($data, "$..*");

var_dump($result);
?>
--EXPECTF--
Warning: JsonPath\JsonPath::find() expects parameter 1 to be array, int given in %s047_php7.php on line %d
NULL
