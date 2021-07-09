--TEST--
Test filter expression with boolean and operator and value true
--SKIPIF--
<?php if (!extension_loaded("jsonpath")) print "skip"; ?>
--FILE--
<?php

$data = [
    [
        "key" => 1,
    ],
    [
        "key" => 3,
    ],
    [
        "key" => "nice",
    ],
    [
        "key" => true,
    ],
    [
        "key" => null,
    ],
    [
        "key" => false,
    ],
    [
        "key" => [],
    ],
    [
        "key" => [],
    ],
    [
        "key" => -1,
    ],
    [
        "key" => 0,
    ],
    [
        "key" => "",
    ],
];

$jsonPath = new \JsonPath\JsonPath();
$result = $jsonPath->find($data, "$[?(@.key>0 && true)]");

var_dump($result);
?>
--EXPECT--
array(2) {
  [0]=>
  array(1) {
    ["key"]=>
    int(1)
  }
  [1]=>
  array(1) {
    ["key"]=>
    int(3)
  }
}