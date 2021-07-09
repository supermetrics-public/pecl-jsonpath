--TEST--
Test array slice with step empty
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

$jsonPath = new \JsonPath\JsonPath();
$result = $jsonPath->find($data, '$[::2]');

var_dump($result);
?>
--EXPECT--
array(3) {
  [0]=>
  string(5) "first"
  [1]=>
  string(5) "third"
  [2]=>
  string(5) "fifth"
}
