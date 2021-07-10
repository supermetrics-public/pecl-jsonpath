--TEST--
Test parens notation
--SKIPIF--
<?php if (!extension_loaded("jsonpath")) print "skip"; ?>
--FILE--
<?php

$data = [
    "key" => 1,
    "some" => 2,
    "more" => 3,
];

$jsonPath = new \JsonPath\JsonPath();
$result = $jsonPath->find($data, "$(key,more)");

var_dump($result);
?>
--EXPECTF--
Fatal error: Uncaught JsonPath\JsonPathException: Unrecognized token `k` at position 2 in %s
Stack trace:
%s
%s
%s