--TEST--
Test bracket notation with quoted string and unescaped single quote
--SKIPIF--
<?php if (!extension_loaded("jsonpath")) print "skip"; ?>
--FILE--
<?php

$data = [
    "single'quote" => "value",
];

$jsonPath = new \JsonPath\JsonPath();
$result = $jsonPath->find($data, "$['single'quote']");

var_dump($result);
?>
--EXPECTF--
Fatal error: Uncaught JsonPath\JsonPathException: Unrecognized token `q` at position 10 in %s
Stack trace:
%s
%s
%s