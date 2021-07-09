--TEST--
Test dot notation with key named null
--SKIPIF--
<?php if (!extension_loaded("jsonpath")) print "skip"; ?>
--FILE--
<?php

$data = [
    "null" => "value",
];

$jsonPath = new \JsonPath\JsonPath();
$result = $jsonPath->find($data, "$.null");

var_dump($result);
?>
--EXPECT--
array(1) {
  [0]=>
  string(5) "value"
}
