--TEST--
Test filter expression after recursive descent
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

$jsonPath = new \JsonPath\JsonPath();
$result = $jsonPath->find($data, "$..[?(@.id==2)]");
sortRecursively($result);

var_dump($result);
?>
--EXPECT--
array(4) {
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
}
