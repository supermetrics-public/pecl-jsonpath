--TEST--
Test dot notation after union with keys
--SKIPIF--
<?php if (!extension_loaded("jsonpath")) print "skip"; ?>
--FILE--
<?php

$data = [
    "one" => [
        "key" => "value",
    ],
    "two" => [
        "k" => "v",
    ],
    "three" => [
        "some" => "more",
        "key" => "other value",
    ],
];

$jsonPath = new \JsonPath\JsonPath();
$result = $jsonPath->find($data, "$['one','three'].key");

var_dump($result);
?>
--EXPECT--
array(2) {
  [0]=>
  string(5) "value"
  [1]=>
  string(11) "other value"
}