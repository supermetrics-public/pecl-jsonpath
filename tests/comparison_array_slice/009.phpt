--TEST--
Test array slice with large number for start end negative step
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
$result = $jsonPath->find($data, '$[113667776004:2:-1]');

var_dump($result);
?>
--EXPECT--
array(2) {
  [0]=>
  string(5) "fifth"
  [1]=>
  string(5) "forth"
}
