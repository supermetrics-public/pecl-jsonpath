--TEST--
Test filter expression with value null
--SKIPIF--
<?php if (!extension_loaded("jsonpath")) print "skip"; ?>
--FILE--
<?php

$data = [
    1,
    3,
    "nice",
    true,
    null,
    false,
    [],
    [],
    -1,
    0,
    "",
];

$jsonPath = new JsonPath();
$result = $jsonPath->find($data, "$[?(null)]");

echo "Assertion 1\n";
var_dump($result);
?>
--EXPECTF--
Fatal error: Uncaught RuntimeException: Unrecognized token 'n' at position 4 in %s
Stack trace:
%s
%s
%s