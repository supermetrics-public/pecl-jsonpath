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
        "key" => 41,
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

var_dump($result);
?>
--EXPECT--
array(1) {
  [0]=>
  array(1) {
    ["key"]=>
    int(42)
  }
}
