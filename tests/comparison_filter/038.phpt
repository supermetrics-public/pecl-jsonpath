--TEST--
Test filter expression with equals number with fraction
--SKIPIF--
<?php if (!extension_loaded("jsonpath")) print "skip"; ?>
--FILE--
<?php

$data = [
    [
        "key" => -12.3,
    ],
    [
        "key" => -0.123,
    ],
    [
        "key" => -12,
    ],
    [
        "key" => 12.3,
    ],
    [
        "key" => 2,
    ],
    [
        "key" => "-0.123e2",
    ],
];

$jsonPath = new JsonPath();
$result = $jsonPath->find($data, "$[?(@.key==-0.123e2)]");

echo "Assertion 1\n";
var_dump($result);
?>
--EXPECT--
Assertion 1
array(1) {
  [0]=>
  array(1) {
    ["key"]=>
    float(-12.3)
  }
}
--XFAIL--
Requires more work on filter expressions
