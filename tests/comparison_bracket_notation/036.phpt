--TEST--
Test bracket notation with two literals separated by dot
--SKIPIF--
<?php if (!extension_loaded("jsonpath")) print "skip"; ?>
--FILE--
<?php

$data = [
    "one" => [
        "key" => "value",
    ],
    "two" => [
        "some" => "more",
        "key" => "other value",
    ],
    "two.some" => "42",
    "two'.'some" => "43",
];

$jsonPath = new \JsonPath\JsonPath();
$result = $jsonPath->find($data, "$['two'.'some']");

var_dump($result);
?>
--EXPECTF--
Fatal error: Uncaught JsonPath\JsonPathException: Quoted node names must use the bracket notation `[` at position 8 in %s
Stack trace:
%s
%s
%s
