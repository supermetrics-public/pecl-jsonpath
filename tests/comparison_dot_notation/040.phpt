--TEST--
Test dot notation with wildcard on empty array
--SKIPIF--
<?php if (!extension_loaded("jsonpath")) print "skip"; ?>
--FILE--
<?php

$data = [];

$jsonPath = new \JsonPath\JsonPath();
$result = $jsonPath->find($data, "$.*");

var_dump($result);
?>
--EXPECT--
bool(false)
