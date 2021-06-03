--TEST--
Test filter expression with equals array for dot notation with star
--SKIPIF--
<?php if (!extension_loaded("jsonpath")) print "skip"; ?>
--FILE--
<?php

$data = [
    [
        1,
        2,
    ],
    [
        2,
        3,
    ],
    [
        1,
    ],
    [
        2,
    ],
    [
        1,
        2,
        3,
    ],
    1,
    2,
    3,
];

$jsonPath = new JsonPath();
$result = $jsonPath->find($data, "$[?(@.*==[1,2])]");

var_dump($result);
?>
--EXPECTF--
Fatal error: Uncaught RuntimeException: Unsupported expression operand in %s
Stack trace:
%s
%s
%s