--TEST--
Test array slice with range of -1
--SKIPIF--
<?php if (!extension_loaded("jsonpath")) print "skip"; ?>
--FILE--
<?php

$data = [
    "first",
    "second",
    "third",
    "forth",
];

$jsonPath = new JsonPath();
$result = $jsonPath->find($data, '$[2:1]');

var_dump($result);
?>
--EXPECT--
bool(false)
