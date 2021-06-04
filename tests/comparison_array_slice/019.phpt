--TEST--
Test array slice with negative step only
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
$result = $jsonPath->find($data, '$[::-2]');

var_dump($result);
?>
--EXPECT--
array(3) {
  [0]=>
  string(5) "fifth"
  [1]=>
  string(5) "third"
  [2]=>
  string(5) "first"
}
