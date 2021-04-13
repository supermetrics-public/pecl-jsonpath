--TEST--
Test filter expression with equals on object
--SKIPIF--
<?php if (!extension_loaded("jsonpath")) print "skip"; ?>
--FILE--
<?php

$data = [
    "a" => [
        "key" => 0,
    ],
    "b" => [
        "key" => 42,
    ],
    "c" => [
        "key" => -1,
    ],
    "d" => [
        "key": 41,
    ],
    "e" => [
        "key" => 43,
    ],
    "f" => [
        "key" => 42.0001,
    ],
    "g" => [
        "key" => 41.9999,
    ],
    "h" => [
        "key" => 100,
    ],
    "i" => [
        "some" => "value",
    ],
];

$jsonPath = new JsonPath();
$result = $jsonPath->find($data, "$[?(@.key==42)]");

echo "Assertion 1\n";
var_dump($result);
?>
--EXPECT--
Assertion 1
array(1) {
  [0]=>
  array(1) {
    ["key"]=>
    int(42)
  }
}
--XFAIL--
Requires more work on filter expressions
