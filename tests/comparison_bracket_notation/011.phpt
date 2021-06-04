--TEST--
Test bracket notation with number
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
$result = $jsonPath->find($data, '$[2]');

var_dump($result);
?>
--EXPECT--
array(1) {
  [0]=>
  string(5) "third"
}
