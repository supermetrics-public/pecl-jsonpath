--TEST--
Test dot notation after bracket notation with wildcard
--SKIPIF--
<?php if (!extension_loaded("jsonpath")) print "skip"; ?>
--FILE--
<?php

$data = [
    [
        "a" => 1,
    ],
    [
        "a" => 1,
    ],
];

$jsonPath = new \JsonPath\JsonPath();
$result = $jsonPath->find($data, "$[*].a");

var_dump($result);
?>
--EXPECT--
array(2) {
  [0]=>
  int(1)
  [1]=>
  int(1)
}
