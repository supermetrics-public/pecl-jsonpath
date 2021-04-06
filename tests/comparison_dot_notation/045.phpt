--TEST--
Test dot notation with wildcard after recursive descent
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

$jsonPath = new JsonPath();
$result = $jsonPath->find($data, "$..*");

echo "Assertion 1\n";
var_dump($result);
?>
--EXPECT--
Assertion 1
array(6) {
  [0]=>
  string(6) "string"
  [1]=>
  string(5) "value"
  [2]=>
  int(0)
  [3]=>
  int(1)
  [4]=>
  array(2) {
    [0]=>
    int(0)
    [1]=>
    int(1)
  }
  [5]=>
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
}
--XFAIL--
Requires more work on the recursive descent implementation
