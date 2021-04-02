--TEST--
Test dot notation on empty object value
--SKIPIF--
<?php if (!extension_loaded("jsonpath")) print "skip"; ?>
--FILE--
<?php

$data = [
    "key" => [],
];

$jsonPath = new JsonPath();
$result = $jsonPath->find($data, "$.key");

echo "Assertion 1\n";
var_dump($result);
?>
--EXPECT--
Assertion 1
array(1) {
  [0]=>
  array(0) {
  }
}
