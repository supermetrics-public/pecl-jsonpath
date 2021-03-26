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

$jsonPath = new JsonPath();
$result = $jsonPath->find($data, '$[::]');

echo "Assertion 1\n";
var_dump($result);
?>
--EXPECT--
Assertion 1
array(2) {
  [0]=>
  string(5) "first"
  [1]=>
  string(6) "second"
}
