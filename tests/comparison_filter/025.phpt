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

$jsonPath = new JsonPath();
$result = $jsonPath->find($data, "$[?(@.key==43)]");

echo "Assertion 1\n";
var_dump($result);
?>
--EXPECT--
Assertion 1
bool(false)
