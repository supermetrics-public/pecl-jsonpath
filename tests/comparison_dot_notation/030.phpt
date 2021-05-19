--TEST--
Test dot notation with key root literal
--SKIPIF--
<?php if (!extension_loaded("jsonpath")) print "skip"; ?>
--FILE--
<?php

$data = [
    "$" => "value",
];

$jsonPath = new JsonPath();
$result = $jsonPath->find($data, "$.$");

echo "Assertion 1\n";
var_dump($result);
?>
--EXPECTF--
Fatal error: Uncaught RuntimeException: Unexpected root `$` in node name, use bracket notation for node names with special characters in %s
Stack trace:
%s
%s
%s
