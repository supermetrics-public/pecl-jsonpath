--TEST--
Test filter expression with current object
--SKIPIF--
<?php if (!extension_loaded("jsonpath")) print "skip"; ?>
--FILE--
<?php

$data = [
    "some value",
    null,
    "value",
    0,
    1,
    -1,
    "",
    [],
    [],
    false,
    true,
];

$jsonPath = new JsonPath();
$result = $jsonPath->find($data, "$[?(@)]");

echo "Assertion 1\n";
var_dump($result);
?>
--EXPECT--
Assertion 1
array(11) {
  [0]=>
  string(10) "some value"
  [1]=>
  NULL
  [2]=>
  string(5) "value"
  [3]=>
  int(0)
  [4]=>
  int(1)
  [5]=>
  int(-1)
  [6]=>
  string(0) ""
  [7]=>
  array(0) {
  }
  [8]=>
  array(0) {
  }
  [9]=>
  bool(false)
  [10]=>
  bool(true)
}