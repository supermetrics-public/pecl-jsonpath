--TEST--
Test filter expression with bracket notation with number
--SKIPIF--
<?php if (!extension_loaded("jsonpath")) print "skip"; ?>
--FILE--
<?php

$data = [
    [
        "a",
        "b",
    ],
    [
        "x",
        "y",
    ],
];

$jsonPath = new JsonPath();
$result = $jsonPath->find($data, "$[?(@[1]=='b')]");

var_dump($result);
?>
--EXPECT--
array(1) {
  [0]=>
  array(2) {
    [0]=>
    string(1) "a"
    [1]=>
    string(1) "b"
  }
}