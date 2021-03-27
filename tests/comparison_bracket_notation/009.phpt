--TEST--
Test bracket notation with empty string doubled quoted
--SKIPIF--
<?php if (!extension_loaded("jsonpath")) print "skip"; ?>
--FILE--
<?php

$data = [
    "" => 42,
    "''" => 123,
    "\"\"" => 222,
];

$jsonPath = new JsonPath();
$result = $jsonPath->find($data, '$[""]');

echo "Assertion 1\n";
var_dump($result);
?>
--EXPECT--
Assertion 1
array(1) {
  [0]=>
  int(42)
}
