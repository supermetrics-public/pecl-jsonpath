--TEST--
Test array slice notation for a node that is a reference
--SKIPIF--
<?php if (!extension_loaded("jsonpath")) print "skip"; ?>
--FILE--
<?php

$data = [
    "key" => 42,
];

$another = [
    43,
    44,
    45,
];

$data["another"] =& $another;

$jsonPath = new \JsonPath\JsonPath();

$result = $jsonPath->find($data, "$.another[1::]");

var_dump($result);
?>
--EXPECT--
array(2) {
  [0]=>
  int(44)
  [1]=>
  int(45)
}