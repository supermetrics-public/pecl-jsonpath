--TEST--
Test filter expression with negation and less than
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
$result = $jsonPath->find($data, "$[?(!(@.key<42))]");

var_dump($result);
?>
--EXPECT--
array(8) {
  [0]=>
  array(1) {
    ["key"]=>
    int(42)
  }
  [1]=>
  array(1) {
    ["key"]=>
    int(43)
  }
  [2]=>
  array(1) {
    ["key"]=>
    float(42.0001)
  }
  [3]=>
  array(1) {
    ["key"]=>
    int(100)
  }
  [4]=>
  array(1) {
    ["key"]=>
    string(2) "43"
  }
  [5]=>
  array(1) {
    ["key"]=>
    string(2) "42"
  }
  [6]=>
  array(1) {
    ["key"]=>
    string(5) "value"
  }
  [7]=>
  array(1) {
    ["some"]=>
    string(5) "value"
  }
}