--TEST--
Test filter with a node that is a reference
--SKIPIF--
<?php if (!extension_loaded("jsonpath")) print "skip"; ?>
--FILE--
<?php

$data = [
    "key" => 42,
];

$another = [
    [
        "key" => 43,
    ],
    [
        "key" => 53,
    ],
];

$data["another"] =& $another;

$jsonPath = new \JsonPath\JsonPath();

$result = $jsonPath->find($data, "$.another[?(@.key <= 50)]");

var_dump($result);
?>
--EXPECT--
array(1) {
  [0]=>
  array(1) {
    ["key"]=>
    int(43)
  }
}