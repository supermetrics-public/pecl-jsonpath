--TEST--
Test dot notation without root and dot
--SKIPIF--
<?php if (!extension_loaded("jsonpath")) print "skip"; ?>
--FILE--
<?php

$data = [
    "key" => "value",
];

$jsonPath = new \JsonPath\JsonPath();
$result = $jsonPath->find($data, "key");

var_dump($result);
?>
--EXPECTF--
Fatal error: Uncaught JsonPath\JsonPathException: Unrecognized token `k` at position 0 in %s
Stack trace:
%s
%s
%s