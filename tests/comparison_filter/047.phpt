--TEST--
Test filter expression with greater than
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
$result = $jsonPath->find($data, "$[?(@.key>42)]");

var_dump($result);
?>
--EXPECT--
array(4) {
  [0]=>
  array(1) {
    ["key"]=>
    int(43)
  }
  [1]=>
  array(1) {
    ["key"]=>
    float(42.0001)
  }
  [2]=>
  array(1) {
    ["key"]=>
    int(100)
  }
  [3]=>
  array(1) {
    ["key"]=>
    string(2) "43"
  }
}