--TEST--
Test dot notation after bracket notation with wildcard on some matching
--SKIPIF--
<?php if (!extension_loaded("jsonpath")) print "skip"; ?>
--FILE--
<?php

$data = [
    [
        "a" => 1,
    ],
    [
        "b" => 1,
    ],
];

$jsonPath = new JsonPath();
$result = $jsonPath->find($data, "$[*].a");

var_dump($result);
?>
--EXPECT--
array(1) {
  [0]=>
  int(1)
}
