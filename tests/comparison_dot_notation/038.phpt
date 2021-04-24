--TEST--
Test dot notation with space padded key
--SKIPIF--
<?php if (!extension_loaded("jsonpath")) print "skip"; ?>
--FILE--
<?php

$data = [
    " a" => 1,
    "a" => 2,
    " a " => 3,
    "" => 4,
];

$jsonPath = new JsonPath();
$result = $jsonPath->find($data, "$. a");

echo "Assertion 1\n";
var_dump($result);
?>
--EXPECT--
PHP Fatal Error
--XFAIL--
Now returns unrecognized token 'a' at position 3, would be better to identify the whitespace as the culprit
