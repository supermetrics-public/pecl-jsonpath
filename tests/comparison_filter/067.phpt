--TEST--
Test filter expression with value
--SKIPIF--
<?php if (!extension_loaded("jsonpath")) print "skip"; ?>
--FILE--
<?php

$data = [
    [
        "some" => "some value",
    ],
    [
        "key" => true,
    ],
    [
        "key" => false,
    ],
    [
        "key" => null,
    ],
    [
        "key" => "value",
    ],
    [
        "key" => "",
    ],
    [
        "key" => 0,
    ],
    [
        "key" => 1,
    ],
    [
        "key" => -1,
    ],
    [
        "key" => 42,
    ],
    [
        "key" => [],
    ],
    [
        "key" => [],
    ],
];

$jsonPath = new JsonPath();
$result = $jsonPath->find($data, "$[?(@.key)]");

var_dump($result);
?>
--EXPECT--
array(11) {
  [0]=>
  array(1) {
    ["key"]=>
    bool(true)
  }
  [1]=>
  array(1) {
    ["key"]=>
    bool(false)
  }
  [2]=>
  array(1) {
    ["key"]=>
    NULL
  }
  [3]=>
  array(1) {
    ["key"]=>
    string(5) "value"
  }
  [4]=>
  array(1) {
    ["key"]=>
    string(0) ""
  }
  [5]=>
  array(1) {
    ["key"]=>
    int(0)
  }
  [6]=>
  array(1) {
    ["key"]=>
    int(1)
  }
  [7]=>
  array(1) {
    ["key"]=>
    int(-1)
  }
  [8]=>
  array(1) {
    ["key"]=>
    int(42)
  }
  [9]=>
  array(1) {
    ["key"]=>
    array(0) {
    }
  }
  [10]=>
  array(1) {
    ["key"]=>
    array(0) {
    }
  }
}
