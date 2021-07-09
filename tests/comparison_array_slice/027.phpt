--TEST--
Test array slice with positive start and negative end and range of -1
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
$result = $jsonPath->find($data, '$[3:-4]');

var_dump($result);
?>
--EXPECT--
bool(false)
