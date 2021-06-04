--TEST--
Test array slice with step 1
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
$result = $jsonPath->find($data, '$[0:3:1]');

var_dump($result);
?>
--EXPECT--
array(3) {
  [0]=>
  string(5) "first"
  [1]=>
  string(6) "second"
  [2]=>
  string(5) "third"
}
