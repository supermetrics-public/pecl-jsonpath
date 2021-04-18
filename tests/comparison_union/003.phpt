--TEST--
Test union with duplication from object
--SKIPIF--
<?php if (!extension_loaded("jsonpath")) print "skip"; ?>
--FILE--
<?php

$data = [
    "a" => 1,
];

$jsonPath = new JsonPath();
$result = $jsonPath->find($data, "$['a','a']");

echo "Assertion 1\n";
var_dump($result);
?>
--EXPECT--
Assertion 1
array(2) {
  [0]=>
  int(1)
  [1]=>
  int(1)
}
--XFAIL--
Requires more work on union implementation
