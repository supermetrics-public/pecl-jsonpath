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

echo "Assertion 1\n";
var_dump($result);
?>
--EXPECT--
Assertion 1
bool(false)
