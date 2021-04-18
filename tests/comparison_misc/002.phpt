--TEST--
Test parens notation
--SKIPIF--
<?php if (!extension_loaded("jsonpath")) print "skip"; ?>
--FILE--
<?php

$data = [
    "key" => 1,
    "some" => 2,
    "more" => 3,
];

$jsonPath = new JsonPath();
$result = $jsonPath->find($data, "$(key,more)");

echo "Assertion 1\n";
var_dump($result);
?>
--EXPECTF--
Fatal error: Uncaught RuntimeException: $ must be followed by a child selector, filter or recurse element. in %s
Stack trace:
%s
%s
%s