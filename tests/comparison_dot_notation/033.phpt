--TEST--
Test dot notation with number on object
--SKIPIF--
<?php if (!extension_loaded("jsonpath")) print "skip"; ?>
--FILE--
<?php

$data = [
    "a" => "first",
    "2" => "second",
    "c" => "third",
];

$jsonPath = new JsonPath();
$result = $jsonPath->find($data, "$.2");

var_dump($result);
?>
--EXPECT--
array(1) {
  [0]=>
  string(6) "second"
}