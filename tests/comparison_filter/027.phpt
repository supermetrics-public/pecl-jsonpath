--TEST--
Test filter expression with equals on object with key matching query
--SKIPIF--
<?php if (!extension_loaded("jsonpath")) print "skip"; ?>
--FILE--
<?php

$data = [
    "id" => 2,
];

$jsonPath = new JsonPath();
$result = $jsonPath->find($data, "$[?(@.id==2)]");

var_dump($result);
?>
--EXPECT--
bool(false)
