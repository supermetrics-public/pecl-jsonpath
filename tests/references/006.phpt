--TEST--
Test recursive descent with a node that is a reference
--SKIPIF--
<?php if (!extension_loaded("jsonpath")) print "skip"; ?>
--FILE--
<?php

$data = [
    "key" => 42,
];

$another = [
    "key" => 43,
];

$data["another"] =& $another;

$jsonPath = new \JsonPath\JsonPath();

$result = $jsonPath->find($data, "$..['key']");

var_dump($result);
?>
--EXPECT--
array(2) {
  [0]=>
  int(42)
  [1]=>
  int(43)
}