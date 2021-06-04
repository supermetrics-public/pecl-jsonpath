--TEST--
Test dot notation with key named in
--SKIPIF--
<?php if (!extension_loaded("jsonpath")) print "skip"; ?>
--FILE--
<?php

$data = [
    "in" => "value",
];

$jsonPath = new JsonPath();
$result = $jsonPath->find($data, "$.in");

var_dump($result);
?>
--EXPECT--
array(1) {
  [0]=>
  string(5) "value"
}
