--TEST--
Test array slice with range of 0
--SKIPIF--
<?php if (!extension_loaded("jsonpath")) print "skip"; ?>
--FILE--
<?php

$data = [
    "first",
    "second",
];

$jsonPath = new JsonPath();
$result = $jsonPath->find($data, '$[0:0]');

var_dump($result);
?>
--EXPECT--
bool(false)
