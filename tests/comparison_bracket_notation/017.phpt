--TEST--
Test bracket notation with number -1 on empty array
--SKIPIF--
<?php if (!extension_loaded("jsonpath")) print "skip"; ?>
--FILE--
<?php

$data = [];

$jsonPath = new \JsonPath\JsonPath();
$result = $jsonPath->find($data, '$[-1]');

var_dump($result);
?>
--EXPECT--
bool(false)
