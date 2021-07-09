--TEST--
Test union with wildcard and number
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

$jsonPath = new \JsonPath\JsonPath();
$result = $jsonPath->find($data, "$[*,1]");

var_dump($result);
?>
--EXPECTF--
Fatal error: Uncaught RuntimeException: Wildcard filter contains an invalid character, expected `]` at position 3 in %s
Stack trace:
%s
%s
%s
