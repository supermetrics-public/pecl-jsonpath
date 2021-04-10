--TEST--
Test dot notation on array
--SKIPIF--
<?php if (!extension_loaded("jsonpath")) print "skip"; ?>
--FILE--
<?php

$data = [
    0,
    1,
];

$jsonPath = new JsonPath();
$result = $jsonPath->find($data, "$.key");

echo "Assertion 1\n";
var_dump($result);
?>
--EXPECT--
Assertion 1
bool(false)
