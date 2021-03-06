--TEST--
Test dot notation with single quotes and dot
--SKIPIF--
<?php if (!extension_loaded("jsonpath")) print "skip"; ?>
--FILE--
<?php

$data = [
    "some.key" => 42,
    "some" => [
        "key" => "value",
    ],
    "'some.key'" => 43,
];

$jsonPath = new \JsonPath\JsonPath();
$result = $jsonPath->find($data, "$.'some.key'");

var_dump($result);
?>
--EXPECTF--
Fatal error: Uncaught JsonPath\JsonPathException: Quoted node names must use the bracket notation `[` at position 2 in %s
Stack trace:
%s
%s
%s
