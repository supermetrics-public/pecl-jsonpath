--TEST--
Test bracket notation with wildcard on empty object
--SKIPIF--
<?php if (!extension_loaded("jsonpath")) print "skip"; ?>
--FILE--
<?php

$data = [];

$jsonPath = new \JsonPath\JsonPath();
$result = $jsonPath->find($data, "$[*]");

var_dump($result);
?>
--EXPECT--
bool(false)
