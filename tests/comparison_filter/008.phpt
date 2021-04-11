--TEST--
Test filter expression with boolean or operator
--SKIPIF--
<?php if (!extension_loaded("jsonpath")) print "skip"; ?>
--FILE--
<?php

$data = [
    [
        "key" => 42,
    ],
    [
        "key" => 43,
    ],
    [
        "key" => 44,
    ],
];

$jsonPath = new JsonPath();
$result = $jsonPath->find($data, "$[?(@.key>43 || @.key<43)]");

echo "Assertion 1\n";
var_dump($result);
?>
--EXPECT--
Assertion 1
array(2) {
  [0]=>
  array(1) {
    ["key"]=>
    int(42)
  }
  [1]=>
  array(1) {
    ["key"]=>
    int(44)
  }
}
--XFAIL--
Requires more work on filter expressions
