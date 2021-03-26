--TEST--
Test array slice with range of 1
--SKIPIF--
<?php if (!extension_loaded("jsonpath")) print "skip"; ?>
--FILE--
<?php

$data = [
    "first",
    "second",
];

$jsonPath = new JsonPath();
$result = $jsonPath->find($data, '$[0:1]');

echo "Assertion 1\n";
var_dump($result);
?>
--EXPECT--
Assertion 1
array(1) {
  [0]=>
  string(5) "first"
}
