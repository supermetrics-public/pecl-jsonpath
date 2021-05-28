--TEST--
Test union with duplication from object
--SKIPIF--
<?php if (!extension_loaded("jsonpath")) print "skip"; ?>
--FILE--
<?php

$data = [
    "a" => 1,
];

$jsonPath = new JsonPath();
$result = $jsonPath->find($data, "$['a','a']");

var_dump($result);
?>
--EXPECT--
array(2) {
  [0]=>
  int(1)
  [1]=>
  int(1)
}