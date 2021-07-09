--TEST--
Test dot notation on empty object value
--SKIPIF--
<?php if (!extension_loaded("jsonpath")) print "skip"; ?>
--FILE--
<?php

$data = [
    "key" => [],
];

$jsonPath = new \JsonPath\JsonPath();
$result = $jsonPath->find($data, "$.key");

var_dump($result);
?>
--EXPECT--
array(1) {
  [0]=>
  array(0) {
  }
}
