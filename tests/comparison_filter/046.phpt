--TEST--
Test filter expression with equals with root reference
--SKIPIF--
<?php if (!extension_loaded("jsonpath")) print "skip"; ?>
--FILE--
<?php

$data = [
    "value" => 42,
    "items" => [
        [
            "key" => 10,
        ],
        [
            "key" => 42,
        ],
        [
            "key" => 50,
        ],
    ],
];

$jsonPath = new \JsonPath\JsonPath();
$result = $jsonPath->find($data, "$.items[?(@.key==$.value)]");

var_dump($result);
?>
--EXPECT--
array(1) {
  [0]=>
  array(1) {
    ["key"]=>
    int(42)
  }
}