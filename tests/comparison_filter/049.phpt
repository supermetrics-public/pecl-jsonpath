--TEST--
Test filter expression with in array of values
--SKIPIF--
<?php if (!extension_loaded("jsonpath")) print "skip"; ?>
--FILE--
<?php

$data = [
    [
        "d" => 1,
    ],
    [
        "d" => 2,
    ],
    [
        "d" => 1,
    ],
    [
        "d" => 3,
    ],
    [
        "d" => 4,
    ],
];

$jsonPath = new JsonPath();
$result = $jsonPath->find($data, "$[?(@.d in [2, 3])]");

echo "Assertion 1\n";
var_dump($result);
?>
--EXPECTF--
Fatal error: Uncaught RuntimeException: Unrecognized token 'i' at position 8 in %s
Stack trace:
%s
%s
%s