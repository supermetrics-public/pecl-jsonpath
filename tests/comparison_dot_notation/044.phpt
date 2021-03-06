--TEST--
Test dot notation with wildcard after dot notation with wildcard on nested arrays
--SKIPIF--
<?php if (!extension_loaded("jsonpath")) print "skip"; ?>
--FILE--
<?php

$data = [
    [
        1,
        2,
        3,
    ],
    [
        4,
        5,
        6,
    ]
];

$jsonPath = new \JsonPath\JsonPath();
$result = $jsonPath->find($data, "$.*.*");

var_dump($result);
?>
--EXPECT--
array(6) {
  [0]=>
  int(1)
  [1]=>
  int(2)
  [2]=>
  int(3)
  [3]=>
  int(4)
  [4]=>
  int(5)
  [5]=>
  int(6)
}
