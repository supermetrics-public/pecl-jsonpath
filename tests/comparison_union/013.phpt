--TEST--
Test union with slice and number
--SKIPIF--
<?php if (!extension_loaded("jsonpath")) print "skip"; ?>
--FILE--
<?php

$data = [
    1,
    2,
    3,
    4,
    5,
];

$jsonPath = new \JsonPath\JsonPath();
$result = $jsonPath->find($data, "$[1:3,4]");

var_dump($result);
?>
--EXPECTF--
Fatal error: Uncaught JsonPath\JsonPathException: Expected slice separator `:` or integer, got LEX_CHILD_SEP in slice union filter in %s
Stack trace:
%s
%s
%s