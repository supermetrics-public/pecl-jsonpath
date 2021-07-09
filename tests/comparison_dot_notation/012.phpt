--TEST--
Test dot notation after bracket notation after recursive descent
--SKIPIF--
<?php if (!extension_loaded("jsonpath")) print "skip"; ?>
--FILE--
<?php
require_once __DIR__ . '/../utils/sort_recursively.php';

$data = [
    "k" => [
        [
            "key" => "some value",
        ],
        [
            "key" => 42,
        ],
    ],
    "kk" => [
        [
            [
                "key" => 100,
            ],
            [
                "key" => 200,
            ],
            [
                "key" => 300,
            ],
            [
                [
                    "key" => 400,
                ],
                [
                    "key" => 500,
                ],
                [
                    "key" => 600,
                ],
            ],
        ],
    ],
    "key" => [
        0,
        1,
    ],
];

$jsonPath = new \JsonPath\JsonPath();
$result = $jsonPath->find($data, "$..[1].key");
sortRecursively($result);

var_dump($result);
?>
--EXPECT--
array(3) {
  [0]=>
  int(42)
  [1]=>
  int(200)
  [2]=>
  int(500)
}
