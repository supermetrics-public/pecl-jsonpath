--TEST--
Test recursive descent after dot notation
--SKIPIF--
<?php if (!extension_loaded("jsonpath")) print "skip"; ?>
--FILE--
<?php

$data = [
    "some key" => "value",
    "key" => [
        "complex" => "string",
        "primitives" => [
            0,
            1,
        ],
    ],
];

$jsonPath = new \JsonPath\JsonPath();
$result = $jsonPath->find($data, "$.key..");

var_dump($result);
?>
--EXPECTF--
Fatal error: Uncaught JsonPath\JsonPathException: Recursive descent operator `..` must be followed by a child selector, filter or wildcard at position 7 in %s
Stack trace:
%s
%s
%s