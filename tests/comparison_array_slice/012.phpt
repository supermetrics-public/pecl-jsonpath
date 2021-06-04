--TEST--
Test array slice with negative start and end and range of 1
--SKIPIF--
<?php if (!extension_loaded("jsonpath")) print "skip"; ?>
--FILE--
<?php

$data = [
    2,
    "a",
    4,
    5,
    100,
    "nice",
];

$jsonPath = new JsonPath();
$result = $jsonPath->find($data, '$[-4:-3]');

var_dump($result);
?>
--EXPECT--
array(1) {
  [0]=>
  int(4)
}
