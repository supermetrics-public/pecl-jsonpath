--TEST--
Test dot notation with single quotes after recursive descent
--SKIPIF--
<?php if (!extension_loaded("jsonpath")) print "skip"; ?>
--FILE--
<?php

$data = [
    "object" => [
        "key" => "value",
        "'key'" => 42,
        "array" => [
            [
                "key" => "something",
                "'key'" => 0,
            ],
            [
                "key" => [
                    "key" => "russian dolls",
                ],
                "'key'" => [
                    "'key'" => 99,
                ],
            ],
        ],
    ],
    "key" => "top",
    "'key'" => 42,
];

$jsonPath = new \JsonPath\JsonPath();
$result = $jsonPath->find($data, "$..'key'");

var_dump($result);
?>
--EXPECTF--
Fatal error: Uncaught JsonPath\JsonPathException: Quoted node names must use the bracket notation `[` at position 3 in %s
Stack trace:
%s
%s
%s