--TEST--
Test dot notation with key named length on array
--SKIPIF--
<?php if (!extension_loaded("jsonpath")) print "skip"; ?>
--FILE--
<?php

$data = [
    4,
    5,
    6,
];

$jsonPath = new \JsonPath\JsonPath();
$result = $jsonPath->find($data, "$.length");

var_dump($result);
?>
--EXPECT--
bool(false)
