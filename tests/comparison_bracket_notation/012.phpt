--TEST--
Test bracket notation with number on object
--SKIPIF--
<?php if (!extension_loaded("jsonpath")) print "skip"; ?>
--FILE--
<?php

$data = [
    "0" => "value",
];

$jsonPath = new JsonPath();
$result = $jsonPath->find($data, '$[0]');

echo "Assertion 1\n";
var_dump($result);
?>
--EXPECT--
Assertion 1
array(1) {
  [0]=>
  string(5) "value"
}
