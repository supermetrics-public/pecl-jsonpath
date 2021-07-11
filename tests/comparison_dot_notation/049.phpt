--TEST--
Test dot notation without root
--SKIPIF--
<?php if (!extension_loaded("jsonpath")) print "skip"; ?>
--FILE--
<?php

$data = [
    "key" => "value",
];

$jsonPath = new \JsonPath\JsonPath();
$result = $jsonPath->find($data, ".key");

var_dump($result);
?>
--EXPECTF--
Fatal error: Uncaught JsonPath\JsonPathException: JSONPath must start with a root operator `$` in %s
Stack trace:
%s
%s
%s