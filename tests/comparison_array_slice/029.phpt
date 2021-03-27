--TEST--
Test array slice with positive start and negative end and range of 1
--SKIPIF--
<?php if (!extension_loaded("jsonpath")) print "skip"; ?>
--FILE--
<?php

$data = [
    2,
    "a",
    4,
    5,
    100,
    "nice",
];

$jsonPath = new JsonPath();
$result = $jsonPath->find($data, '$[3:-2]');

echo "Assertion 1\n";
var_dump($result);
?>
--EXPECT--
Assertion 1
array(1) {
  [0]=>
  int(5)
}
