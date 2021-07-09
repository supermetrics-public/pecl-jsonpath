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

$jsonPath = new \JsonPath\JsonPath();
$result = $jsonPath->find($data, "$.id");

var_dump($result);
?>
--EXPECT--
bool(false)
