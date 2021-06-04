--TEST--
Test array slice with step but end not aligned
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
$result = $jsonPath->find($data, '$[0:4:2]');

var_dump($result);
?>
--EXPECT--
array(2) {
  [0]=>
  string(5) "first"
  [1]=>
  string(5) "third"
}
