--TEST--
Test filter expression with multiplication
--SKIPIF--
<?php if (!extension_loaded("jsonpath")) print "skip"; ?>
--FILE--
<?php

$data = [
    [
        "key" => 60,
    ],
    [
        "key" => 50,
    ],
    [
        "key" => 10,
    ],
    [
        "key" => -50,
    ],
    [
        "key*2" => 100,
    ],
];

$jsonPath = new \JsonPath\JsonPath();
$result = $jsonPath->find($data, "$[?(@.key*2==100)]");

var_dump($result);
?>
--EXPECTF--
Fatal error: Uncaught JsonPath\JsonPathException: Unrecognized token `*` at position 9 in %s
Stack trace:
%s
%s
%s
