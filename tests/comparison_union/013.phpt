--TEST--
Test union with slice and number
--SKIPIF--
<?php if (!extension_loaded("jsonpath")) print "skip"; ?>
--FILE--
<?php

$data = [
    1,
    2,
    3,
    4,
    5,
];

$jsonPath = new JsonPath();
$result = $jsonPath->find($data, "$[1:3,4]");

echo "Assertion 1\n";
var_dump($result);
?>
--EXPECTF--
Fatal error: Uncaught RuntimeException: Multiple filter list separators found [,:], only one type is allowed. in %s
Stack trace:
%s
%s
%s