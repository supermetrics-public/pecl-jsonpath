--TEST--
Test dot notation with wildcard after recursive descent
--SKIPIF--
<?php if (!extension_loaded("jsonpath")) print "skip"; ?>
--FILE--
<?php
require_once __DIR__ . '/../utils/sort_recursively.php';

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
sortRecursively($result);

var_dump($result);
?>
--EXPECT--
array(6) {
  [0]=>
  array(2) {
    [0]=>
    int(0)
    [1]=>
    int(1)
  }
  [1]=>
  array(2) {
    ["primitives"]=>
    array(2) {
      [0]=>
      int(0)
      [1]=>
      int(1)
    }
    ["complex"]=>
    string(6) "string"
  }
  [2]=>
  int(0)
  [3]=>
  int(1)
  [4]=>
  string(6) "string"
  [5]=>
  string(5) "value"
}
