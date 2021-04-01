--TEST--
Test bracket notation with number -1
--SKIPIF--
<?php if (!extension_loaded("jsonpath")) print "skip"; ?>
--FILE--
<?php

$data = [
    "first",
    "second",
    "third",
];

$jsonPath = new JsonPath();
$result = $jsonPath->find($data, '$[-1]');

echo "Assertion 1\n";
var_dump($result);
?>
--EXPECT--
Assertion 1
array(1) {
  [0]=>
  string(5) "third"
}
