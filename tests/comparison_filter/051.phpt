--TEST--
Test filter expression with less than
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

$jsonPath = new JsonPath();
$result = $jsonPath->find($data, "$[?(@.key<42)]");

echo "Assertion 1\n";
var_dump($result);
?>
--EXPECT--
Assertion 1
array(4) {
  [0]=>
  array(1) {
    ["key"]=>
    int(0)
  }
  [1]=>
  array(1) {
    ["key"]=>
    int(-1)
  }
  [2]=>
  array(1) {
    ["key"]=>
    int(41)
  }
  [3]=>
  array(1) {
    ["key"]=>
    float(41.9999)
  }
}
--XFAIL--
Outcome depends on how values of different types are compared
