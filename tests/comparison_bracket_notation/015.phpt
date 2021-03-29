--TEST--
Test bracket notation with number after dot notation with wildcard on nested arrays with different length
--SKIPIF--
<?php if (!extension_loaded("jsonpath")) print "skip"; ?>
--FILE--
<?php

$data = [
    [
        1,
    ],
    [
        2,
        3,
    ],
];

$jsonPath = new JsonPath();
$result = $jsonPath->find($data, '$.*[1]');

echo "Assertion 1\n";
var_dump($result);
?>
--EXPECT--
Assertion 1
array[1]{
  [0]=>
  int(3)
}
--XFAIL--
Requires filter expression support in wildcard iteration
