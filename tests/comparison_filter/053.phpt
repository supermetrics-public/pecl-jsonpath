--TEST--
Test filter expression with multiplication
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
        "key*2" => 100,
    ],
];

$jsonPath = new JsonPath();
$result = $jsonPath->find($data, "$[?(@.key*2==100)]");

echo "Assertion 1\n";
var_dump($result);
?>
--EXPECTF--
Fatal error: Uncaught RuntimeException: Multiplying node values is not supported. in %s
Stack trace:
%s
%s
%s
