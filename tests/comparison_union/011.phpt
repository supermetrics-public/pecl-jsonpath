--TEST--
Test union with numbers in decreasing order
--SKIPIF--
<?php if (!extension_loaded("jsonpath")) print "skip"; ?>
--FILE--
<?php

$data = [
    1,
    2,
    3,
    4,
    5,
];

$jsonPath = new JsonPath();
$result = $jsonPath->find($data, "$[4,1]");

var_dump($result);
?>
--EXPECT--
array(2) {
  [0]=>
  int(5)
  [1]=>
  int(2)
}
