--TEST--
Test bracket notation after recursive descent
--SKIPIF--
<?php if (!extension_loaded("jsonpath")) print "skip"; ?>
--FILE--
<?php
require_once __DIR__ . '/../utils/sort_recursively.php';

$data = [
    "first",
    [
        "key" => [
            "first nested",
            [
                "more" => [
                    [
                        "nested" => [
                            "deepest",
                            "second",
                        ],
                    ],
                    [
                        "more",
                        "values",
                    ],
                ],
            ],
        ],
    ],
];

$jsonPath = new \JsonPath\JsonPath();
$result = $jsonPath->find($data, '$..[0]');
sortRecursively($result);

var_dump($result);
?>
--EXPECT--
array(5) {
  [0]=>
  array(1) {
    ["nested"]=>
    array(2) {
      [0]=>
      string(7) "deepest"
      [1]=>
      string(6) "second"
    }
  }
  [1]=>
  string(7) "deepest"
  [2]=>
  string(5) "first"
  [3]=>
  string(12) "first nested"
  [4]=>
  string(4) "more"
}
