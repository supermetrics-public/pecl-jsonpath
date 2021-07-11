--TEST--
Test dot notation with key root literal
--SKIPIF--
<?php if (!extension_loaded("jsonpath")) print "skip"; ?>
--FILE--
<?php

$data = [
    "$" => "value",
];

$jsonPath = new \JsonPath\JsonPath();
$result = $jsonPath->find($data, "$.$");

var_dump($result);
?>
--EXPECTF--
Fatal error: Uncaught JsonPath\JsonPathException: Unexpected root `$` in node name, use bracket notation for node names with special characters in %s
Stack trace:
%s
%s
%s
