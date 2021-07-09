--TEST--
Test array slice with start large negative number and open end on short array
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
$result = $jsonPath->find($data, '$[-4:]');

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
