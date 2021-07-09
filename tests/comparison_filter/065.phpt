--TEST--
Test filter expression with tautological comparison
--SKIPIF--
<?php if (!extension_loaded("jsonpath")) print "skip"; ?>
--FILE--
<?php

$data = [
    1,
    3,
    "nice",
    true,
    null,
    false,
    [],
    [],
    -1,
    0,
    "",
];

$jsonPath = new \JsonPath\JsonPath();
$result = $jsonPath->find($data, "$[?(1==1)]");

var_dump($result);
?>
--EXPECT--
array(11) {
  [0]=>
  int(1)
  [1]=>
  int(3)
  [2]=>
  string(4) "nice"
  [3]=>
  bool(true)
  [4]=>
  NULL
  [5]=>
  bool(false)
  [6]=>
  array(0) {
  }
  [7]=>
  array(0) {
  }
  [8]=>
  int(-1)
  [9]=>
  int(0)
  [10]=>
  string(0) ""
}
