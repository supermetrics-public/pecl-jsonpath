--TEST--
Test array slice with range of 1
--SKIPIF--
<?php if (!extension_loaded("jsonpath")) print "skip"; ?>
--FILE--
<?php

$data = [
    "first",
    "second",
];

$jsonPath = new \JsonPath\JsonPath();
$result = $jsonPath->find($data, '$[0:1]');

var_dump($result);
?>
--EXPECT--
array(1) {
  [0]=>
  string(5) "first"
}
