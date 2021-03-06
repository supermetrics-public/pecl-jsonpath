--TEST--
Test bracket notation with wildcard on array
--SKIPIF--
<?php if (!extension_loaded("jsonpath")) print "skip"; ?>
--FILE--
<?php

$data = [
    "string",
    42,
    [
        "key" => "value",
    ],
    [
        0,
        1,
    ],
];

$jsonPath = new \JsonPath\JsonPath();
$result = $jsonPath->find($data, "$[*]");

var_dump($result);
?>
--EXPECT--
array(4) {
  [0]=>
  string(6) "string"
  [1]=>
  int(42)
  [2]=>
  array(1) {
    ["key"]=>
    string(5) "value"
  }
  [3]=>
  array(2) {
    [0]=>
    int(0)
    [1]=>
    int(1)
  }
}
