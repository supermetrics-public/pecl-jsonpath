--TEST--
Test bracket notation with wildcard after dot notation after bracket notation with wildcard
--SKIPIF--
<?php if (!extension_loaded("jsonpath")) print "skip"; ?>
--FILE--
<?php

$data = [
    "key" => "value",
    "another key" => [
        "complex" => "string",
        "primitives" => [
            0,
            1,
        ],
    ],
];

$jsonPath = new \JsonPath\JsonPath();
$result = $jsonPath->find($data, "$..[*]");

var_dump($result);
?>
--EXPECT--
array(6) {
  [0]=>
  string(5) "value"
  [1]=>
  array(2) {
    ["complex"]=>
    string(6) "string"
    ["primitives"]=>
    array(2) {
      [0]=>
      int(0)
      [1]=>
      int(1)
    }
  }
  [2]=>
  string(6) "string"
  [3]=>
  array(2) {
    [0]=>
    int(0)
    [1]=>
    int(1)
  }
  [4]=>
  int(0)
  [5]=>
  int(1)
}
