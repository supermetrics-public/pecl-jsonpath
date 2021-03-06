--TEST--
Test recursive descent
--SKIPIF--
<?php if (!extension_loaded("jsonpath")) print "skip"; ?>
--FILE--
<?php

$data = [
    [
        "a" => [
            "b" => "c",
        ],
    ],
    [
        0,
        1,
    ],
];

$jsonPath = new \JsonPath\JsonPath();
$result = $jsonPath->find($data, "$..");

var_dump($result);
?>
--EXPECTF--
Fatal error: Uncaught JsonPath\JsonPathException: Recursive descent operator `..` must be followed by a child selector, filter or wildcard at position 3 in %s
Stack trace:
%s
%s
%s