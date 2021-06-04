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

var_dump($result);
?>
--EXPECT--
bool(false)
