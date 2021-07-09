--TEST--
Test union with keys on object without key
--SKIPIF--
<?php if (!extension_loaded("jsonpath")) print "skip"; ?>
--FILE--
<?php

$data = [
    "key" => "value",
    "another" => "entry",
];

$jsonPath = new \JsonPath\JsonPath();
$result = $jsonPath->find($data, "$['missing','key']");

var_dump($result);
?>
--EXPECT--
array(1) {
  [0]=>
  string(5) "value"
}