--TEST--
Test array slice with negative step and start greater than end
--SKIPIF--
<?php if (!extension_loaded("jsonpath")) print "skip"; ?>
--FILE--
<?php

$data = [
    "first",
    "second",
    "third",
    "forth",
    "fifth",
];

$jsonPath = new JsonPath();
$result = $jsonPath->find($data, '$[0:3:-2]');

var_dump($result);
?>
--EXPECT--
bool(false)
