--TEST--
Test bracket notation with quoted dot wildcard
--SKIPIF--
<?php if (!extension_loaded("jsonpath")) print "skip"; ?>
--FILE--
<?php

$data = [
    "key" => 42,
    ".*" => 1,
    "" => 10,
];

$jsonPath = new JsonPath();
$result = $jsonPath->find($data, "$['.*']");

echo "Assertion 1\n";
var_dump($result);
?>
--EXPECT--
Assertion 1
array(1) {
  [0]=>
  int(1)
}
