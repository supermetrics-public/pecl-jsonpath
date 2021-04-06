--TEST--
Test dot notation with wildcard after recursive descent on null value array
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
$result = $jsonPath->find($data, "$..*");

echo "Assertion 1\n";
var_dump($result);
?>
--EXPECT--
Assertion 1
array(3) {
  [0]=>
  int(40)
  [1]=>
  int(42)
  [2]=>
  NULL
}
--XFAIL--
Requires more work on the recursive descent implementation
