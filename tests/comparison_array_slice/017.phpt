--TEST--
Test array slice with negative step on partially overlapping array
--SKIPIF--
<?php if (!extension_loaded("jsonpath")) print "skip"; ?>
--FILE--
<?php

$data = [
    "first",
    "second",
    "third",
    "forth",
    "fifth",
];

$jsonPath = new JsonPath();
$result = $jsonPath->find($data, '$[7:3:-1]');

echo "Assertion 1\n";
var_dump($result);
?>
--EXPECT--
Assertion 1
array(1) {
  [0]=>
  string(5) "fifth"
}
