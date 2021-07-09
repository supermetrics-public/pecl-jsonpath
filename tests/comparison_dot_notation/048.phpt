--TEST--
Test dot notation without dot
--SKIPIF--
<?php if (!extension_loaded("jsonpath")) print "skip"; ?>
--FILE--
<?php

$data = [
    "a" => 1,
    "\$a" => 2,
];

$jsonPath = new \JsonPath\JsonPath();
$result = $jsonPath->find($data, "\$a");

var_dump($result);
?>
--EXPECTF--
Fatal error: Uncaught RuntimeException: Unrecognized token `a` at position 1 in %s
Stack trace:
%s
%s
%s
