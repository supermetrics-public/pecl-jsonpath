--TEST--
Test dot notation after recursive descent after dot notation
--SKIPIF--
<?php if (!extension_loaded("jsonpath")) print "skip"; ?>
--FILE--
<?php
require_once __DIR__ . '/../utils/sort_recursively.php';

$data = [
    "store" => [
        "book" => [
            [
                "category" => "reference",
                "author" => "Nigel Rees",
                "title" => "Sayings of the Century",
                "price" => 8.95,
            ],
            [
                "category" => "fiction",
                "author" => "Evelyn Waugh",
                "title" => "Sword of Honour",
                "price" => 12.99,
            ],
            [
                "category" => "fiction",
                "author" => "Herman Melville",
                "title" => "Moby Dick",
                "isbn" => "0-553-21311-3",
                "price" => 8.99,
            ],
            [
                "category" => "fiction",
                "author" => "J. R. R. Tolkien",
                "title" => "The Lord of the Rings",
                "isbn" => "0-395-19395-8",
                "price" => 22.99,
            ],
        ],
        "bicycle" => [
            "color" => "red",
            "price" => 19.95,
        ],
    ],
];

$jsonPath = new \JsonPath\JsonPath();
$result = $jsonPath->find($data, "$.store..price");
sortRecursively($result);

var_dump($result);
?>
--EXPECT--
array(5) {
  [0]=>
  float(8.95)
  [1]=>
  float(8.99)
  [2]=>
  float(12.99)
  [3]=>
  float(19.95)
  [4]=>
  float(22.99)
}
