--TEST--
Test dot notation with space padded key
--SKIPIF--
<?php if (!extension_loaded("jsonpath")) print "skip"; ?>
--FILE--
<?php

$data = [
    " a" => 1,
    "a" => 2,
    " a " => 3,
    "" => 4,
];

$jsonPath = new \JsonPath\JsonPath();
$result = $jsonPath->find($data, "$. a");

var_dump($result);
?>
--EXPECTF--
Fatal error: Uncaught JsonPath\JsonPathException: Unexpected whitespace at position 2 in %s
Stack trace:
%s
%s
%s
