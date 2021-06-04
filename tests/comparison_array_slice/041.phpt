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

$jsonPath = new JsonPath();
$result = $jsonPath->find($data, '$[1:3:]');

var_dump($result);
?>
--EXPECT--
array(2) {
  [0]=>
  string(6) "second"
  [1]=>
  string(5) "third"
}
