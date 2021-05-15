--TEST--
Test empty
--SKIPIF--
<?php if (!extension_loaded("jsonpath")) print "skip"; ?>
--FILE--
<?php

$data = [
    "a" => 42,
    "" => 21,
];

$jsonPath = new JsonPath();
$result = $jsonPath->find($data, "");

echo "Assertion 1\n";
var_dump($result);
?>
--EXPECTF--
Fatal error: Uncaught RuntimeException: JSONPath expression contains no valid elements in %s
Stack trace:
%s
%s
%s