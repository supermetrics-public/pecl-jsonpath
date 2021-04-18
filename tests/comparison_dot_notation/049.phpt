--TEST--
Test dot notation without root
--SKIPIF--
<?php if (!extension_loaded("jsonpath")) print "skip"; ?>
--FILE--
<?php

$data = [
    "key" => "value",
];

$jsonPath = new JsonPath();
$result = $jsonPath->find($data, ".key");

echo "Assertion 1\n";
var_dump($result);
?>
--EXPECTF--
Fatal error: Uncaught RuntimeException: JSONpath must start with a root $ in %s
Stack trace:
%s
%s
%s