--TEST--
Test array slice with start -2 and open end
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
$result = $jsonPath->find($data, '$[-2:]');

var_dump($result);
?>
--EXPECT--
array(2) {
  [0]=>
  string(6) "second"
  [1]=>
  string(5) "third"
}
