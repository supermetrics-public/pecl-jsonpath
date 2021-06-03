--TEST--
Test filter expression with equals array for array slice with range 1
--SKIPIF--
<?php if (!extension_loaded("jsonpath")) print "skip"; ?>
--FILE--
<?php

$data = [
    [
        1,
        2,
        3,
    ],
    [
        1,
    ],
    [
        2,
        3,
    ],
    1,
    2,
];

$jsonPath = new JsonPath();
$result = $jsonPath->find($data, "$[?(@[0:1]==[1])]");

var_dump($result);
?>
--EXPECTF--
Fatal error: Uncaught RuntimeException: Unsupported expression operand in %s
Stack trace:
%s
%s
%s