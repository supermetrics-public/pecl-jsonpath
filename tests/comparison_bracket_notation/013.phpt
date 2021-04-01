--TEST--
Test bracket notation with number on short array
--SKIPIF--
<?php if (!extension_loaded("jsonpath")) print "skip"; ?>
--FILE--
<?php

$data = [
    "one element",
];

$jsonPath = new JsonPath();
$result = $jsonPath->find($data, '$[1]');

echo "Assertion 1\n";
var_dump($result);
?>
--EXPECT--
Assertion 1
bool(false)
