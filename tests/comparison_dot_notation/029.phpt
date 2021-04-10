--TEST--
Test dot notation with key named true
--SKIPIF--
<?php if (!extension_loaded("jsonpath")) print "skip"; ?>
--FILE--
<?php

$data = [
    "true" => "value",
];

$jsonPath = new JsonPath();
$result = $jsonPath->find($data, "$.true");

echo "Assertion 1\n";
var_dump($result);
?>
--EXPECT--
Assertion 1
array(1) {
  [0]=>
  string(5) "value"
}
