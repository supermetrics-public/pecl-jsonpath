--TEST--
Test dot notation on null value
--SKIPIF--
<?php if (!extension_loaded("jsonpath")) print "skip"; ?>
--FILE--
<?php

$data = [
    "key" => null,
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
  NULL
}
