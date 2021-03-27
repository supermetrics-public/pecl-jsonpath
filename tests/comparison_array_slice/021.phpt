--TEST--
Test array slice with open end and negative step
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
$result = $jsonPath->find($data, '$[3::-1]');

echo "Assertion 1\n";
var_dump($result);
?>
--EXPECT--
Assertion 1
array(4) {
  [0]=>
  string(5) "forth"
  [1]=>
  string(5) "third"
  [2]=>
  string(6) "second"
  [3]=>
  string(5) "first"
}
