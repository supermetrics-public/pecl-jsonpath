--TEST--
Test filter expression with equals number with leading zeros
--SKIPIF--
<?php if (!extension_loaded("jsonpath")) print "skip"; ?>
--FILE--
<?php

$data = [
    [
        "key" => "010",
    ],
    [
        "key" => "10",
    ],
    [
        "key" => 10,
    ],
    [
        "key" => 0,
    ],
    [
        "key" => 8,
    ],
];

$jsonPath = new JsonPath();
$result = $jsonPath->find($data, "$[?(@.key==010)]");

var_dump($result);
?>
--EXPECT--
array(1) {
  [0]=>
  array(1) {
    ["key"]=>
    int(10)
  }
}