--TEST--
Test filter expression with equals on array of numbers
--SKIPIF--
<?php if (!extension_loaded("jsonpath")) print "skip"; ?>
--FILE--
<?php

$data = [
    0,
    42,
    -1,
    41,
    43,
    42.0001,
    41.9999,
    null,
    100,
];

$jsonPath = new JsonPath();
$result = $jsonPath->find($data, "$[?(@==42)]");

echo "Assertion 1\n";
var_dump($result);
?>
--EXPECT--
Assertion 1
array(1) {
  [0]=>
  int(42)
}
--XFAIL--
Requires more work on filter expressions
