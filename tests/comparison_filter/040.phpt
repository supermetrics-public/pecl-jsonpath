--TEST--
Test filter expression with equals object
--SKIPIF--
<?php if (!extension_loaded("jsonpath")) print "skip"; ?>
--DESCRIPTION--
Test a dictionary as an expression operand. This is currently unrecognized by
the grammar, so a lexing error is returned. Dictionary operand support can be
implemented in the future if users signal that this is generally useful.
--FILE--
<?php

$data = [
    [
        "d" => [
            "k" => "v",
        ],
    ],
    [
        "d" => [
            "a" => "b",
        ],
    ],
    [
        "d" => "k",
    ],
    [
        "d" => "v",
    ],
    [
        "d" => [
        ],
    ],
    [
        "d" => [
        ],
    ],
    [
        "d" => null,
    ],
    [
        "d" => -1,
    ],
    [
        "d" => 0,
    ],
    [
        "d" => 1,
    ],
    [
        "d" => "[object Object]",
    ],
    [
        "d" => "{\"k\": \"v\"}",
    ],
    [
        "d" => "{\"k\":\"v\"}",
    ],
    "v",
];

$jsonPath = new JsonPath();
$result = $jsonPath->find($data, "$[?(@.d=={\"k\":\"v\"})]");

var_dump($result);
?>
--EXPECTF--
Fatal error: Uncaught RuntimeException: Unrecognized token `{` at position 9 in %s
Stack trace:
%s
%s
%s