--TEST--
Test filter expression with division
--SKIPIF--
<?php if (!extension_loaded("jsonpath")) print "skip"; ?>
--FILE--
<?php

$data = [
    [
        "key" => 60,
    ],
    [
        "key" => 50,
    ],
    [
        "key" => 10,
    ],
    [
        "key" => -50,
    ],
    [
        "key/10" => 5,
    ],
];

$jsonPath = new JsonPath();
$result = $jsonPath->find($data, "$[?(@.key/10==5)]");

echo "Assertion 1\n";
var_dump($result);
?>
--EXPECTF--
Fatal error: Uncaught RuntimeException: Unrecognized token `/` at position 9 in %s
Stack trace:
%s
%s
%s