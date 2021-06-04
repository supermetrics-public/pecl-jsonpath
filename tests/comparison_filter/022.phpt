--TEST--
Test filter expression with empty expression
--SKIPIF--
<?php if (!extension_loaded("jsonpath")) print "skip"; ?>
--FILE--
<?php

$data = [
    1,
    [
        "key" => 42,
    ],
    "value",
    null,
];

$jsonPath = new JsonPath();
$result = $jsonPath->find($data, "$[?()]");

var_dump($result);
?>
--EXPECTF--
Fatal error: Uncaught RuntimeException: Expecting child node, filter, expression, or recursive node in %s
Stack trace:
%s
%s
%s