--TEST--
Test union with keys
--SKIPIF--
<?php if (!extension_loaded("jsonpath")) print "skip"; ?>
--FILE--
<?php

$data = [
    "key" => "value",
    "another" => "entry",
];

$jsonPath = new JsonPath();
$result = $jsonPath->find($data, "$['key','another']");

var_dump($result);
?>
--EXPECT--
array(2) {
  [0]=>
  string(5) "value"
  [1]=>
  string(5) "entry"
}