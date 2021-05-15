--TEST--
Test filter expression without parens
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
            "key" => 42,
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
$result = $jsonPath->find($data, "$[?@.key==42]");

echo "Assertion 1\n";
var_dump($result);
?>
--EXPECTF--
Fatal error: Uncaught RuntimeException: Missing opening paren ( in %s
Stack trace:
%s
%s
%s
