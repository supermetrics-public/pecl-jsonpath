--TEST--
Test array slice with step empty
--SKIPIF--
<?php if (!extension_loaded("jsonpath")) print "skip"; ?>
--FILE--
<?php

$data = [
    "first",
    "second",
    "third",
    "forth",
    "fifth",
];

$jsonPath = new JsonPath();
$result = $jsonPath->find($data, '$[1:3:]');

echo "Assertion 1\n";
var_dump($result);
?>
--EXPECT--
Assertion 1
array(2) {
  [0]=>
  string(6) "second"
  [1]=>
  string(5) "third"
}
