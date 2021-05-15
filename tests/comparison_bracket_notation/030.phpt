--TEST--
Test bracket notation with quoted string and unescaped single quote
--SKIPIF--
<?php if (!extension_loaded("jsonpath")) print "skip"; ?>
--FILE--
<?php

$data = [
    "single'quote" => "value",
];

$jsonPath = new JsonPath();
$result = $jsonPath->find($data, "$['single'quote']");

echo "Assertion 1\n";
var_dump($result);
?>
--EXPECTF--
Fatal error: Uncaught RuntimeException: Missing closing bracket `]` at position %d in %s
Stack trace:
%s
%s
%s