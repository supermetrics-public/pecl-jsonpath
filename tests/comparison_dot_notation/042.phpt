--TEST--
Test dot notation with wildcard on object
--SKIPIF--
<?php if (!extension_loaded("jsonpath")) print "skip"; ?>
--FILE--
<?php
require_once __DIR__ . '/../utils/sort_recursively.php';

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

$jsonPath = new \JsonPath\JsonPath();
$result = $jsonPath->find($data, "$.*");
sortRecursively($result);

var_dump($result);
?>
--EXPECT--
array(4) {
  [0]=>
  array(1) {
    ["key"]=>
    string(5) "value"
  }
  [1]=>
  array(2) {
    [0]=>
    int(0)
    [1]=>
    int(1)
  }
  [2]=>
  int(42)
  [3]=>
  string(6) "string"
}
