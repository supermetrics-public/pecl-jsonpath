--TEST--
Test bracket notation with number -1
--SKIPIF--
<?php if (!extension_loaded("jsonpath")) print "skip"; ?>
--FILE--
<?php

$data = [
    "first",
    "second",
    "third",
];

$jsonPath = new JsonPath();
$result = $jsonPath->find($data, '$[-1]');

var_dump($result);
?>
--EXPECT--
array(1) {
  [0]=>
  string(5) "third"
}
