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

echo "Assertion 1\n";
var_dump($result);
?>
--EXPECT--
Assertion 1
bool(false)
