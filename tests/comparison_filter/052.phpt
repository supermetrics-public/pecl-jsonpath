--TEST--
Test filter expression with less than or equal
--SKIPIF--
<?php if (!extension_loaded("jsonpath")) print "skip"; ?>
--FILE--
<?php

$data = [
    [
        "key" => 0,
    ],
    [
        "key" => 42,
    ],
    [
        "key" => -1,
    ],
    [
        "key" => 41,
    ],
    [
        "key" => 43,
    ],
    [
        "key" => 42.0001,
    ],
    [
        "key" => 41.9999,
    ],
    [
        "key" => 100,
    ],
    [
        "key" => "43",
    ],
    [
        "key" => "42",
    ],
    [
        "key" => "41",
    ],
    [
        "key" => "value",
    ],
    [
        "some" => "value",
    ],
];

$jsonPath = new \JsonPath\JsonPath();
$result = $jsonPath->find($data, "$[?(@.key<=42)]");

var_dump($result);
?>
--EXPECT--
array(5) {
  [0]=>
  array(1) {
    ["key"]=>
    int(0)
  }
  [1]=>
  array(1) {
    ["key"]=>
    int(42)
  }
  [2]=>
  array(1) {
    ["key"]=>
    int(-1)
  }
  [3]=>
  array(1) {
    ["key"]=>
    int(41)
  }
  [4]=>
  array(1) {
    ["key"]=>
    float(41.9999)
  }
}