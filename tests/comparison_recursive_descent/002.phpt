--TEST--
Test recursive descent after dot notation
--SKIPIF--
<?php if (!extension_loaded("jsonpath")) print "skip"; ?>
--FILE--
<?php

$data = [
    "some key" => "value",
    "key" => [
        "complex" => "string",
        "primitives" => [
            0,
            1,
        ],
    ],
];

$jsonPath = new JsonPath();
$result = $jsonPath->find($data, "$.key..");

echo "Assertion 1\n";
var_dump($result);
?>
--EXPECTF--
Fatal error: Uncaught RuntimeException: Recursive descent operator `..` must be followed by a child selector, filter or wildcard in %s
Stack trace:
%s
%s
%s