--TEST--
Test bracket notation with dot
--SKIPIF--
<?php if (!extension_loaded("jsonpath")) print "skip"; ?>
--FILE--
<?php

$data = [
    "one" => [
        "key" => "value",
    ],
    "two" => [
        "some" => "more",
        "key" => "other value",
    ],
    "two.some" => 42,
];

$jsonPath = new \JsonPath\JsonPath();
$result = $jsonPath->find($data, "$['two.some']");

var_dump($result);
?>
--EXPECT--
array(1) {
  [0]=>
  int(42)
}
