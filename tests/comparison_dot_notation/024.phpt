--TEST--
Test dot notation with empty path
--SKIPIF--
<?php if (!extension_loaded("jsonpath")) print "skip"; ?>
--FILE--
<?php

$data = [
    "key" => 42,
    "" => 9001,
    "''" => "nice",
];

$jsonPath = new \JsonPath\JsonPath();
$result = $jsonPath->find($data, "$.");

var_dump($result);
?>
--EXPECTF--
Fatal error: Uncaught JsonPath\JsonPathException: Dot selector `.` must be followed by a node name or wildcard at position 1 in %s
Stack trace:
%s
%s
%s
