--TEST--
Test filter expression with single equal
--SKIPIF--
<?php if (!extension_loaded("jsonpath")) print "skip"; ?>
--FILE--
<?php

$data = [
    [
        "key" => 0,
    ],
    [
        "key" => 42,
    ],
    [
        "key" => -1,
    ],
    [
        "key" => 1,
    ],
    [
        "key" => 41,
    ],
    [
        "key" => 43,
    ],
    [
        "key" => 42.0001,
    ],
    [
        "key" => 41.9999,
    ],
    [
        "key" => 100,
    ],
    [
        "key" => "some",
    ],
    [
        "key" => "42",
    ],
    [
        "key" => null,
    ],
    [
        "key" => 420,
    ],
    [
        "key" => "",
    ],
    [
        "key" => [],
    ],
    [
        "key" => [],
    ],
    [
        "key" => [
            42,
        ],
    ],
    [
        "key" => [
            "key" =>  42,
        ],
    ],
    [
        "key" => [
            "some" => 42,
        ],
    ],
    [
        "some" => "value",
    ],
];

$jsonPath = new JsonPath();
$result = $jsonPath->find($data, "$[?(@.key=42)]");

var_dump($result);
?>
--EXPECTF--
Fatal error: Uncaught RuntimeException: Invalid character after `=`, valid values are `==` and `=~` at position 10 in %s
Stack trace:
%s
%s
%s
