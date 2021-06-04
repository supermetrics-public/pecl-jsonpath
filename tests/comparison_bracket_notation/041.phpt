--TEST--
Test bracket notation with wildcard on null value array
--SKIPIF--
<?php if (!extension_loaded("jsonpath")) print "skip"; ?>
--FILE--
<?php

$data = [
    40,
    null,
    42,
];

$jsonPath = new JsonPath();
$result = $jsonPath->find($data, "$[*]");

var_dump($result);
?>
--EXPECT--
array(3) {
  [0]=>
  int(40)
  [1]=>
  NULL
  [2]=>
  int(42)
}
