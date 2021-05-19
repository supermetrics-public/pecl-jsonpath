--TEST--
Test script expression
--SKIPIF--
<?php if (!extension_loaded("jsonpath")) print "skip"; ?>
--FILE--
<?php

$data = [
    "first",
    "second",
    "third",
    "forth",
    "fifth",
];

$jsonPath = new JsonPath();
$result = $jsonPath->find($data, "$[(@.length-1)]");

var_dump($result);
?>
--EXPECTF--
Fatal error: Uncaught RuntimeException: Unexpected filter element in %s
Stack trace:
%s
%s
%s