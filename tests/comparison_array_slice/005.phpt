--TEST--
Test array slice on partially overlapping array
--SKIPIF--
<?php if (!extension_loaded("jsonpath")) print "skip"; ?>
--FILE--
<?php

$data = [
    "first",
    "second",
    "third",
];

$jsonPath = new \JsonPath\JsonPath();
$result = $jsonPath->find($data, '$[1:10]');

var_dump($result);
?>
--EXPECT--
array(2) {
  [0]=>
  string(6) "second"
  [1]=>
  string(5) "third"
}
