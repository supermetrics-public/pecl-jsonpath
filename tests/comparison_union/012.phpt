--TEST--
Test union with repeated matches after dot notation with wildcard
--SKIPIF--
<?php if (!extension_loaded("jsonpath")) print "skip"; ?>
--FILE--
<?php

$data = [
    "a" => [
        "string",
        null,
        true,
    ],
    "b" => [
        false,
        "string",
        5.4,
    ],
];

$jsonPath = new \JsonPath\JsonPath();
$result = $jsonPath->find($data, "$.*[0,:5]");

var_dump($result);
?>
--EXPECTF--
Fatal error: Uncaught JsonPath\JsonPathException: Expected integer, got LEX_SLICE in index union filter in %s
Stack trace:
%s
%s
%s