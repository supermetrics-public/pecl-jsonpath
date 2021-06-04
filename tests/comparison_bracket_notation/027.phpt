--TEST--
Test bracket notation with quoted number on object
--SKIPIF--
<?php if (!extension_loaded("jsonpath")) print "skip"; ?>
--FILE--
<?php

$data = [
    "0" => "value",
];

$jsonPath = new JsonPath();
$result = $jsonPath->find($data, "$['0']");

var_dump($result);
?>
--EXPECT--
array(1) {
  [0]=>
  string(5) "value"
}
