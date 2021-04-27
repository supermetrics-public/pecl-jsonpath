--TEST--
Test filter expression with equals false
--SKIPIF--
<?php if (!extension_loaded("jsonpath")) print "skip"; ?>
--FILE--
<?php

$data = [
    [
        "some" => "some value",
    ],
    [
        "key" => true,
    ],
    [
        "key" => false,
    ],
    [
        "key" => null,
    ],
    [
        "key" => "value",
    ],
    [
        "key" => "",
    ],
    [
        "key" => 0,
    ],
    [
        "key" => 1,
    ],
    [
        "key" => -1,
    ],
    [
        "key" => "42",
    ],
    [
        "key" => [],
    ],
    [
        "key" => [],
    ],
];

$jsonPath = new JsonPath();
$result = $jsonPath->find($data, "$[?(@.key==false)]");

echo "Assertion 1\n";
var_dump($result);
?>
--EXPECT--
Assertion 1
array(1) {
  [0]=>
  array(1) {
    ["key"]=>
    bool(false)
  }
}