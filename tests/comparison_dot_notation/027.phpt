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

$jsonPath = new JsonPath();
$result = $jsonPath->find($data, "$.length");

echo "Assertion 1\n";
var_dump($result);
?>
--EXPECT--
Assertion 1
bool(false)
