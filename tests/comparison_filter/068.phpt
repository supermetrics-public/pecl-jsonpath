--TEST--
Test filter expression with value after dot notation with wildcard on array of objects
--SKIPIF--
<?php if (!extension_loaded("jsonpath")) print "skip"; ?>
--FILE--
<?php

$data = [
    [
        "some" => "some value",
    ],
    [
        "key" => "value",
    ],
];

$jsonPath = new \JsonPath\JsonPath();
$result = $jsonPath->find($data, "$.*[?(@.key)]");

var_dump($result);
?>
--EXPECT--
bool(false)
