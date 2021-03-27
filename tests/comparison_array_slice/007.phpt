--TEST--
Test array slice with large number for end and negative step
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
$result = $jsonPath->find($data, '$[2:-113667776004:-1]');

echo "Assertion 1\n";
var_dump($result);
?>
--EXPECT--
Assertion 1
array(3) {
  [0]=>
  string(5) "third"
  [1]=>
  string(6) "second"
  [2]=>
  string(5) "first"
}
