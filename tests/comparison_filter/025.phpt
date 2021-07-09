--TEST--
Test filter expression with equals on array without match
--SKIPIF--
<?php if (!extension_loaded("jsonpath")) print "skip"; ?>
--FILE--
<?php

$data = [
    [
        "key" => 42,
    ]
];

$jsonPath = new \JsonPath\JsonPath();
$result = $jsonPath->find($data, "$[?(@.key==43)]");

var_dump($result);
?>
--EXPECT--
bool(false)
