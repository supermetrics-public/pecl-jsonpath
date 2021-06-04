--TEST--
Test filter expression with equals number for array slice with range 1
--SKIPIF--
<?php if (!extension_loaded("jsonpath")) print "skip"; ?>
--FILE--
<?php

$data = [
    [
        1,
        2,
        3,
    ],
    [
        1,
    ],
    [
        2,
        3,
    ],
    1,
    2,
];

$jsonPath = new JsonPath();
$result = $jsonPath->find($data, "$[?(@[0:1]==1)]");

var_dump($result);
?>
--EXPECT--
array(2) {
  [0]=>
  array(3) {
    [0]=>
    int(1)
    [1]=>
    int(2)
    [2]=>
    int(3)
  }
  [1]=>
  array(1) {
    [0]=>
    int(1)
  }
}