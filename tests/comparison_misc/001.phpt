--TEST--
Test empty
--SKIPIF--
<?php if (!extension_loaded("jsonpath")) print "skip"; ?>
--FILE--
<?php

$data = [
    "a" => 42,
    "" => 21,
];

$jsonPath = new \JsonPath\JsonPath();
$result = $jsonPath->find($data, "");

var_dump($result);
?>
--EXPECTF--
Fatal error: Uncaught JsonPath\JsonPathException: JSONPath must start with a root operator `$` in %s
Stack trace:
%s
%s
%s