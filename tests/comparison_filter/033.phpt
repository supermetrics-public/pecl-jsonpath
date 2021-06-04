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

var_dump($result);
?>
--EXPECT--
array(1) {
  [0]=>
  array(1) {
    ["key"]=>
    bool(false)
  }
}