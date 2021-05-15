--TEST--
Test dot notation with double quotes
--SKIPIF--
<?php if (!extension_loaded("jsonpath")) print "skip"; ?>
--FILE--
<?php

$data = [
    "key" => "value",
    "\"key\"" => 42,
];

$jsonPath = new JsonPath();
$result = $jsonPath->find($data, '$."key"');

echo "Assertion 1\n";
var_dump($result);
?>
--EXPECTF--
Fatal error: Uncaught RuntimeException: Quoted node names must use the bracket notation [ at position 2 in %s
Stack trace:
%s
%s
%s
