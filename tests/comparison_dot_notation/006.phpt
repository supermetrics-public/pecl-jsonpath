--TEST--
Test dot notation on array value
--SKIPIF--
<?php if (!extension_loaded("jsonpath")) print "skip"; ?>
--FILE--
<?php

$data = [
    "key" => [
        "first",
        "second",
    ],
];

$jsonPath = new JsonPath();
$result = $jsonPath->find($data, "$.key");

var_dump($result);
?>
--EXPECT--
array(1) {
  [0]=>
  array(2) {
    [0]=>
    string(5) "first"
    [1]=>
    string(6) "second"
  }
}
