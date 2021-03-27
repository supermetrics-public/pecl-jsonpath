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

echo "Assertion 1\n";
var_dump($result);
?>
--EXPECT--
Assertion 1
bool(false)
