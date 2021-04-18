--TEST--
Test union with duplication from array
--SKIPIF--
<?php if (!extension_loaded("jsonpath")) print "skip"; ?>
--FILE--
<?php

$data = [
    "a",
];

$jsonPath = new JsonPath();
$result = $jsonPath->find($data, "$[0,0]");

echo "Assertion 1\n";
var_dump($result);
?>
--EXPECT--
Assertion 1
array(2) {
  [0]=>
  string(1) "a"
  [1]=>
  string(1) "a"
}
