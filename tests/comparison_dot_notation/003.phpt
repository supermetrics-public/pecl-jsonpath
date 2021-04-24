--TEST--
Test dot bracket notation without quotes
--SKIPIF--
<?php if (!extension_loaded("jsonpath")) print "skip"; ?>
--FILE--
<?php

$data = [
    "key" => "value",
    "other" => [
        "key" => [
            "key" => 42,
        ],
    ],
];

$jsonPath = new JsonPath();
$result = $jsonPath->find($data, "$.[key]");

echo "Assertion 1\n";
var_dump($result);
?>
--EXPECTF--
Fatal error: Uncaught RuntimeException: Unrecognized token 'k' at position 3 in %s
Stack trace:
%s
%s
%s