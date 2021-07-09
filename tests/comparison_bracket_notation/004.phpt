--TEST--
Test bracket notation with NFC path on NFD key
--SKIPIF--
<?php if (!extension_loaded("jsonpath")) print "skip"; ?>
--FILE--
<?php

$data = [
    "ü" => 42,
];

$jsonPath = new \JsonPath\JsonPath();
$result = $jsonPath->find($data, "$['ü']");

var_dump($result);
?>
--EXPECT--
bool(false)
