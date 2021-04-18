--TEST--
Test filter expression with bracket notation with number
--SKIPIF--
<?php if (!extension_loaded("jsonpath")) print "skip"; ?>
--FILE--
<?php

$data = [
    [
        "a",
        "b",
    ],
    [
        "x",
        "y",
    ],
];

$jsonPath = new JsonPath();
$result = $jsonPath->find($data, "$[?(@[1]=='b')]");

echo "Assertion 1\n";
var_dump($result);
?>
--EXPECT--
Assertion 1
array(1) {
  [0]=>
  array(2) {
    [0]=>
    string(1) "a"
    [1]=>
    string(1) "b"
  }
}
--XFAIL--
Requires more work on filter expressions
