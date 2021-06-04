--TEST--
Test filter expression with boolean or operator and value true
--SKIPIF--
<?php if (!extension_loaded("jsonpath")) print "skip"; ?>
--FILE--
<?php

$data = [
    [
        "key" => 1,
    ],
    [
        "key" => 3,
    ],
    [
        "key" => "nice",
    ],
    [
        "key" => true,
    ],
    [
        "key" => null,
    ],
    [
        "key" => false,
    ],
    [
        "key" => [],
    ],
    [
        "key" => [],
    ],
    [
        "key" => -1,
    ],
    [
        "key" => 0,
    ],
    [
        "key" => "",
    ],
];

$jsonPath = new JsonPath();
$result = $jsonPath->find($data, "$[?(@.key>0 || true)]");

var_dump($result);
?>
--EXPECT--
array(11) {
  [0]=>
  array(1) {
    ["key"]=>
    int(1)
  }
  [1]=>
  array(1) {
    ["key"]=>
    int(3)
  }
  [2]=>
  array(1) {
    ["key"]=>
    string(4) "nice"
  }
  [3]=>
  array(1) {
    ["key"]=>
    bool(true)
  }
  [4]=>
  array(1) {
    ["key"]=>
    NULL
  }
  [5]=>
  array(1) {
    ["key"]=>
    bool(false)
  }
  [6]=>
  array(1) {
    ["key"]=>
    array(0) {
    }
  }
  [7]=>
  array(1) {
    ["key"]=>
    array(0) {
    }
  }
  [8]=>
  array(1) {
    ["key"]=>
    int(-1)
  }
  [9]=>
  array(1) {
    ["key"]=>
    int(0)
  }
  [10]=>
  array(1) {
    ["key"]=>
    string(0) ""
  }
}
