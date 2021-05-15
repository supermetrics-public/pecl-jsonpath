--TEST--
Test root
--SKIPIF--
<?php if (!extension_loaded("jsonpath")) print "skip"; ?>
--FILE--
<?php
require_once __DIR__ . '/../utils/sort_recursively.php';

$data = [
    "key" => "value",
    "another key" => [
        "complex" => [
            "a",
            1,
        ],
    ],
];

$jsonPath = new JsonPath();
$result = $jsonPath->find($data, "$");
sortRecursively($result);

var_dump($result);
?>
--EXPECT--
array(1) {
  [0]=>
  array(2) {
    ["another key"]=>
    array(1) {
      ["complex"]=>
      array(2) {
        [0]=>
        int(1)
        [1]=>
        string(1) "a"
      }
    }
    ["key"]=>
    string(5) "value"
  }
}