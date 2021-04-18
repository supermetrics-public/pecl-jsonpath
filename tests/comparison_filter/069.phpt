--TEST--
Test filter expression with value after recursive descent
--SKIPIF--
<?php if (!extension_loaded("jsonpath")) print "skip"; ?>
--FILE--
<?php
require_once __DIR__ . '/../utils/sort_recursively.php';

$data = [
    "id" => 2,
    "more" => [
        [
            "id" => 2,
        ],
        [
            "more" => [
                "id" => 2,
            ],
        ],
        [
            "id" => [
                "id" => 2,
            ],
        ],
        [
            [
                "id" => 2,
            ],
        ],
    ],
];

$jsonPath = new JsonPath();
$result = $jsonPath->find($data, "$..[?(@.id)]");
sortRecursively($result);

echo "Assertion 1\n";
var_dump($result);
?>
--EXPECT--
Assertion 1
array(5) {
  [0]=>
  array(1) {
    ["id"]=>
    int(2)
  }
  [1]=>
  array(1) {
    ["id"]=>
    int(2)
  }
  [2]=>
  array(1) {
    ["id"]=>
    int(2)
  }
  [3]=>
  array(1) {
    ["id"]=>
    int(2)
  }
  [4]=>
  array(1) {
    ["id"]=>
    array(1) {
      ["id"]=>
      int(2)
    }
  }
}
