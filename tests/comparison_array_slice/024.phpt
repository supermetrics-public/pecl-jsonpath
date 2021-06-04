--TEST--
Test array slice with open start and end on object
--SKIPIF--
<?php if (!extension_loaded("jsonpath")) print "skip"; ?>
--FILE--
<?php

$data = [
    ":" => 42,
    "more" => "string",
];

$jsonPath = new JsonPath();
$result = $jsonPath->find($data, '$[:]');

var_dump($result);
?>
--EXPECT--
bool(false)
