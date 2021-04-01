--TEST--
Test bracket notation with wildcard after array slice
--SKIPIF--
<?php if (!extension_loaded("jsonpath")) print "skip"; ?>
--FILE--
<?php

$data = [
    [
        1,
        2,
    ],
    [
        "a",
        "b",
    ],
    [
        0,
        0,
    ],
];

$jsonPath = new JsonPath();
$result = $jsonPath->find($data, "$[0:2][*]");

echo "Assertion 1\n";
var_dump($result);
?>
--EXPECT--
Assertion 1
array(4) {
  [0]=>
  int(1)
  [1]=>
  int(2)
  [2]=>
  string(1) "a"
  [3]=>
  string(1) "b"
}
