--TEST--
Test dot notation for a node that is a reference
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

$result = $jsonPath->find($data, "$.another.key");

var_dump($result);
?>
--EXPECT--
array(1) {
  [0]=>
  int(43)
}