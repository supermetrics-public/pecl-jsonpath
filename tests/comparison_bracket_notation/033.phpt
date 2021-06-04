--TEST--
Test bracket notation with quoted wildcard literal on object without key
--SKIPIF--
<?php if (!extension_loaded("jsonpath")) print "skip"; ?>
--FILE--
<?php

$data = [
    "another" => "entry",
];

$jsonPath = new JsonPath();
$result = $jsonPath->find($data, "$['*']");

var_dump($result);
?>
--EXPECT--
bool(false)
