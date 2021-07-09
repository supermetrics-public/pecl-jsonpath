--TEST--
Test bracket notation with negative number on short array
--SKIPIF--
<?php if (!extension_loaded("jsonpath")) print "skip"; ?>
--FILE--
<?php

$data = [
    "one element",
];

$jsonPath = new \JsonPath\JsonPath();
$result = $jsonPath->find($data, '$[-2]');

var_dump($result);
?>
--EXPECT--
bool(false)
