--TEST--
Test union with keys after recursive descent
--SKIPIF--
<?php if (!extension_loaded("jsonpath")) print "skip"; ?>
--FILE--
<?php
require_once __DIR__ . '/../utils/sort_recursively.php';

$data = [
    [
        "c" => "cc1",
        "d" => "dd1",
        "e" => "ee1",
    ],
    [
        "c" => "cc2",
        "child" => [
            "d" => "dd2",
        ],
    ],
    [
        "c" => "cc3",
    ],
    [
        "d" => "dd4",
    ],
    [
        "child" => [
            "c" => "cc5",
        ],
    ],
];

$jsonPath = new \JsonPath\JsonPath();
$result = $jsonPath->find($data, "$..['c','d']");
sortRecursively($result);

var_dump($result);
?>
--EXPECT--
array(7) {
  [0]=>
  string(3) "cc1"
  [1]=>
  string(3) "cc2"
  [2]=>
  string(3) "cc3"
  [3]=>
  string(3) "cc5"
  [4]=>
  string(3) "dd1"
  [5]=>
  string(3) "dd2"
  [6]=>
  string(3) "dd4"
}