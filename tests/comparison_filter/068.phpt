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

$jsonPath = new JsonPath();
$result = $jsonPath->find($data, "$.*[?(@.key)]");

echo "Assertion 1\n";
var_dump($result);
?>
--EXPECT--
Assertion 1
bool(false)
