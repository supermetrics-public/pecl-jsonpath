--TEST--
Test filter expression with unsupported operand (int array)
--SKIPIF--
<?php if (!extension_loaded("jsonpath")) print "skip"; ?>
--FILE--
<?php

$data = [
    [
        "d" => [
            "100",
            "200",
        ],
    ],
    [
        "d" => [
            "a",
            "b",
        ],
    ],
    [
        "d" => "v1",
    ],
    [
        "d" => "v2",
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
        "d" => "['v1','v2']",
    ],
    [
        "d" => "['v1', 'v2']",
    ],
    [
        "d" => "v1,v2",
    ],
    [
        "d" => "[\"v1\", \"v2\"]",
    ],
    [
        "d" => "[\"v1\",\"v2\"]",
    ],
];

$jsonPath = new JsonPath();
$result = $jsonPath->find($data, '$[?(@.d==[100,200])]');

var_dump($result);
?>
--EXPECTF--
Fatal error: Uncaught RuntimeException: Unsupported expression operand in %s
Stack trace:
%s
%s
%s