--TEST--
Test filter expression after dot notation with wildcard after recursive descent
--SKIPIF--
<?php if (!extension_loaded("jsonpath")) print "skip"; ?>
--FILE--
<?php
require_once __DIR__ . '/../utils/sort_recursively.php';

$data = [
    [
        "complext" => [
            "one" => [
                [
                    "name" => "first",
                    "id" => 1,
                ],
                [
                    "name" => "next",
                    "id" => 2,
                ],
                [
                    "name" => "another",
                    "id" => 3,
                ],
                [
                    "name" => "more",
                    "id" => 4,
                ],
            ],
            "more" => [
                "name" => "next to last",
                "id" => 5,
            ],
        ],
    ],
    [
        "name" => "last",
        "id" => 6,
    ],
];

$jsonPath = new JsonPath();
$result = $jsonPath->find($data, "$..*[?(@.id>2)]");
sortRecursively($result);

var_dump($result);
?>
--EXPECT--
array(3) {
  [0]=>
  array(2) {
    ["id"]=>
    int(3)
    ["name"]=>
    string(7) "another"
  }
  [1]=>
  array(2) {
    ["id"]=>
    int(4)
    ["name"]=>
    string(4) "more"
  }
  [2]=>
  array(2) {
    ["id"]=>
    int(5)
    ["name"]=>
    string(12) "next to last"
  }
}