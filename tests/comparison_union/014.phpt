--TEST--
Test union with spaces
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
$result = $jsonPath->find($data, "$[ 0 , 1 ]");

var_dump($result);
?>
--EXPECT--
array(2) {
  [0]=>
  string(5) "first"
  [1]=>
  string(6) "second"
}
