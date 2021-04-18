--TEST--
Test bracket notation with wildcard on object
--SKIPIF--
<?php if (!extension_loaded("jsonpath")) print "skip"; ?>
--FILE--
<?php

$data = [
    "some" => "string",
    "int" => 42,
    "object" => [
        "key" => "value",
    ],
    "array" => [
        0,
        1,
    ],
];

$jsonPath = new JsonPath();
$result = $jsonPath->find($data, "$[*]");

echo "Assertion 1\n";
var_dump($result);
?>
--EXPECT--
Assertion 1
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
