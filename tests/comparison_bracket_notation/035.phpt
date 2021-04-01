--TEST--
Test bracket notation with string including dot wildcard
--SKIPIF--
<?php if (!extension_loaded("jsonpath")) print "skip"; ?>
--FILE--
<?php

$data = [
    "nice" => 42,
    "ni.*" => 1,
    "mice" => 100,
];

$jsonPath = new JsonPath();
$result = $jsonPath->find($data, "$['ni.*']");

echo "Assertion 1\n";
var_dump($result);
?>
--EXPECT--
Assertion 1
array(1) {
  [0]=>
  int(1)
}
