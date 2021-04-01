--TEST--
Test bracket notation with NFC path on NFD key
--SKIPIF--
<?php if (!extension_loaded("jsonpath")) print "skip"; ?>
--FILE--
<?php

$data = [
    "ü" => 42,
];

$jsonPath = new JsonPath();
$result = $jsonPath->find($data, "$['ü']");

echo "Assertion 1\n";
var_dump($result);
?>
--EXPECT--
Assertion 1
bool(false)
