--TEST--
Test bracket notation with wildcard on empty object
--SKIPIF--
<?php if (!extension_loaded("jsonpath")) print "skip"; ?>
--FILE--
<?php

$data = [];

$jsonPath = new JsonPath();
$result = $jsonPath->find($data, "$[*]");

echo "Assertion 1\n";
var_dump($result);
?>
--EXPECT--
Assertion 1
bool(false)
