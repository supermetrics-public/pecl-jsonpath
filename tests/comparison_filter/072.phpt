--TEST--
Test filter expression with value null
--SKIPIF--
<?php if (!extension_loaded("jsonpath")) print "skip"; ?>
--FILE--
<?php

$data = [
    1,
    3,
    "nice",
    true,
    null,
    false,
    [],
    [],
    -1,
    0,
    "",
];

$jsonPath = new \JsonPath\JsonPath();
$result = $jsonPath->find($data, "$[?(null)]");

var_dump($result);
?>
--EXPECTF--
Fatal error: Uncaught JsonPath\JsonPathException: Invalid expression. in %s
Stack trace:
%s
%s
%s