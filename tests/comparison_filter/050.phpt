--TEST--
Test filter expression with in current object
--SKIPIF--
<?php if (!extension_loaded("jsonpath")) print "skip"; ?>
--FILE--
<?php

$data = [
    [
        "d" => [
            1,
            2,
            3,
        ],
    ],
    [
        "d" => [
            2,
        ],
    ],
    [
        "d" => [
            1,
        ],
    ],
    [
        "d" => [
            3,
            4,
        ],
    ],
    [
        "d" => [
            4,
            2,
        ],
    ],
];

$jsonPath = new JsonPath();
$result = $jsonPath->find($data, "$[?(2 in @.d)]");

echo "Assertion 1\n";
var_dump($result);
?>
--EXPECTF--
Fatal error: Uncaught RuntimeException: Unrecognized token 'i' at position 6 in %s
Stack trace:
%s
%s
%s