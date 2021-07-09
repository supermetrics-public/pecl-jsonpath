--TEST--
Test array slice with large number for start
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
$result = $jsonPath->find($data, '$[-113667776004:2]');

var_dump($result);
?>
--EXPECT--
array(2) {
  [0]=>
  string(5) "first"
  [1]=>
  string(6) "second"
}
