--TEST--
Test dot notation on null value
--SKIPIF--
<?php if (!extension_loaded("jsonpath")) print "skip"; ?>
--FILE--
<?php

$data = [
    "key" => null,
];

$jsonPath = new \JsonPath\JsonPath();
$result = $jsonPath->find($data, "$.key");

var_dump($result);
?>
--EXPECT--
array(1) {
  [0]=>
  NULL
}
