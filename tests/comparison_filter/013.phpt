--TEST--
Test filter expression with bracket notation with -1
--SKIPIF--
<?php if (!extension_loaded("jsonpath")) print "skip"; ?>
--FILE--
<?php

$data = [
    [
        2,
        3,
    ],
    [
        "a",
    ],
    [
        0,
        2,
    ],
    [
        2,
    ],
];

$jsonPath = new JsonPath();
$result = $jsonPath->find($data, "$[?(@[-1]==2)]");

var_dump($result);
?>
--EXPECT--
array(2) {
  [0]=>
  array(2) {
    [0]=>
    int(0)
    [1]=>
    int(2)
  }
  [1]=>
  array(1) {
    [0]=>
    int(2)
  }
}