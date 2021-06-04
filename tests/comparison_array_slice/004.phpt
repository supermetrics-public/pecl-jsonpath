--TEST--
Test array slice on object
--SKIPIF--
<?php if (!extension_loaded("jsonpath")) print "skip"; ?>
--FILE--
<?php

$data = [
    ":" => 42,
    "more" => "string",
    "a" => 1,
    "b" => 2,
    "c" => 3,
    "1:3" => "nice",
];

$jsonPath = new JsonPath();
$result = $jsonPath->find($data, '$[1:3]');

var_dump($result);
?>
--EXPECT--
bool(false)
