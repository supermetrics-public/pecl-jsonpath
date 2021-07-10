--TEST--
Test bracket notation with empty path
--SKIPIF--
<?php if (!extension_loaded("jsonpath")) print "skip"; ?>
--FILE--
<?php

$data = [
    "" => 42,
    "''" => 123,
    "\"\"" => 222,
];

$jsonPath = new \JsonPath\JsonPath();
$result = $jsonPath->find($data, '$[]');

var_dump($result);
?>
--EXPECTF--
Fatal error: Uncaught JsonPath\JsonPathException: Filter must not be empty in %s007.php:%d
Stack trace:
%s
%s
%s
