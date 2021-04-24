--TEST--
Test bracket notation with two literals separated by dot
--SKIPIF--
<?php if (!extension_loaded("jsonpath")) print "skip"; ?>
--FILE--
<?php

$data = [
    "one" => [
        "key" => "value",
    ],
    "two" => [
        "some" => "more",
        "key" => "other value",
    ],
    "two.some" => "42",
    "two'.'some" => "43",
];

$jsonPath = new JsonPath();
$result = $jsonPath->find($data, "$['two'.'some']");

echo "Assertion 1\n";
var_dump($result);
?>
--EXPECTF--
Fatal error: Uncaught RuntimeException: Missing closing ] bracket at position 7 in %s
Stack trace:
%s
%s
%s
