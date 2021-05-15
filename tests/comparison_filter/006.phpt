--TEST--
Test filter expression with boolean and operator and value false
--SKIPIF--
<?php if (!extension_loaded("jsonpath")) print "skip"; ?>
--FILE--
<?php

$data = [
    [
        "key" => 1,
    ],
    [
        "key" => 3,
    ],
    [
        "key" => "nice",
    ],
    [
        "key" => true,
    ],
    [
        "key" => null,
    ],
    [
        "key" => false,
    ],
    [
        "key" => [],
    ],
    [
        "key" => [],
    ],
    [
        "key" => -1,
    ],
    [
        "key" => 0,
    ],
    [
        "key" => "",
    ],
];

$jsonPath = new JsonPath();
$result = $jsonPath->find($data, "$[?(@.key>0 && false)]");

echo "Assertion 1\n";
var_dump($result);
?>
--EXPECT--
Assertion 1
bool(false)
