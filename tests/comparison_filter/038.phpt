--TEST--
Test filter expression with equals number with fraction
--SKIPIF--
<?php if (!extension_loaded("jsonpath")) print "skip"; ?>
--FILE--
<?php

$data = [
    [
        "key" => -12.3,
    ],
    [
        "key" => -0.123,
    ],
    [
        "key" => -12,
    ],
    [
        "key" => 12.3,
    ],
    [
        "key" => 2,
    ],
    [
        "key" => "-0.123e2",
    ],
];

$jsonPath = new \JsonPath\JsonPath();
$result = $jsonPath->find($data, "$[?(@.key==-0.123e2)]");

var_dump($result);
?>
--EXPECT--
array(1) {
  [0]=>
  array(1) {
    ["key"]=>
    float(-12.3)
  }
}