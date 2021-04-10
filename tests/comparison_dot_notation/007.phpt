--TEST--
Test dot notation on array with containing object matching key
--SKIPIF--
<?php if (!extension_loaded("jsonpath")) print "skip"; ?>
--FILE--
<?php

$data = [
    [
        "id" => 2,
    ],
];

$jsonPath = new JsonPath();
$result = $jsonPath->find($data, "$.id");

echo "Assertion 1\n";
var_dump($result);
?>
--EXPECT--
Assertion 1
bool(false)
