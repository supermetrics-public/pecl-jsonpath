--TEST--
Test array slice with open start and end and step empty
--SKIPIF--
<?php if (!extension_loaded("jsonpath")) print "skip"; ?>
--FILE--
<?php

$data = [
    "first",
    "second",
];

$jsonPath = new \JsonPath\JsonPath();
$result = $jsonPath->find($data, '$[::]');

var_dump($result);
?>
--EXPECT--
array(2) {
  [0]=>
  string(5) "first"
  [1]=>
  string(6) "second"
}
