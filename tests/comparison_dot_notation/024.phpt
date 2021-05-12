--TEST--
Test dot notation with empty path
--SKIPIF--
<?php if (!extension_loaded("jsonpath")) print "skip"; ?>
--FILE--
<?php

$data = [
    "key" => 42,
    "" => 9001,
    "''" => "nice",
];

$jsonPath = new JsonPath();
$result = $jsonPath->find($data, "$.");

echo "Assertion 1\n";
var_dump($result);
?>
--EXPECTF--
Fatal error: Uncaught RuntimeException: Dot selector must be followed by a node name or wildcard at position 1 in %s
Stack trace:
%s
%s
%s
