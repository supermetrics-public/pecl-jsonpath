--TEST--
Test bracket notation with number after dot notation with wildcard on nested arrays with different length
--SKIPIF--
<?php if (!extension_loaded("jsonpath")) print "skip"; ?>
--FILE--
<?php

$data = [
    [
        1,
    ],
    [
        2,
        3,
    ],
];

$jsonPath = new \JsonPath\JsonPath();
$result = $jsonPath->find($data, '$.*[1]');

var_dump($result);
?>
--EXPECT--
array(1) {
  [0]=>
  int(3)
}
