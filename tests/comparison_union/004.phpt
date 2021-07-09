--TEST--
Test union with filter
--SKIPIF--
<?php if (!extension_loaded("jsonpath")) print "skip"; ?>
--FILE--
<?php

$data = [
    [
        "key" => 1,
    ],
    [
        "key" => 8,
    ],
    [
        "key" => 3,
    ],
    [
        "key" => 10,
    ],
    [
        "key" => 7,
    ],
    [
        "key" => 2,
    ],
    [
        "key" => 6,
    ],
    [
        "key" => 4,
    ],
];

$jsonPath = new \JsonPath\JsonPath();
$result = $jsonPath->find($data, "$[?(@.key<3),?(@.key>6)]");

var_dump($result);
?>
--EXPECTF--
Fatal error: Uncaught RuntimeException: Unrecognized token `?` at position 13 in %s
Stack trace:
%s
%s
%s
