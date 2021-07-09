--TEST--
Test array slice with negative start and positive end and range of 0
--SKIPIF--
<?php if (!extension_loaded("jsonpath")) print "skip"; ?>
--FILE--
<?php

$data = [
    2,
    "a",
    4,
    5,
    100,
    "nice",
];

$jsonPath = new \JsonPath\JsonPath();
$result = $jsonPath->find($data, '$[-4:2]');

var_dump($result);
?>
--EXPECT--
bool(false)
