--TEST--
Test array slice with negative step
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
$result = $jsonPath->find($data, '$[3:0:-2]');

var_dump($result);
?>
--EXPECT--
array(2) {
  [0]=>
  string(5) "forth"
  [1]=>
  string(6) "second"
}
