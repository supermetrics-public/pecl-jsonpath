--TEST--
Test filter expression with bracket notation and current object literal
--SKIPIF--
<?php if (!extension_loaded("jsonpath")) print "skip"; ?>
--FILE--
<?php

$data = [
    [
        "@key" => 0,
    ],
    [
        "@key" => 42,
    ],
    [
        "key" => 42,
    ],
    [
        "@key" => 43,
    ],
    [
        "some" => "value",
    ],
];

$jsonPath = new JsonPath();
$result = $jsonPath->find($data, "$[?(@['@key']==42)]");

var_dump($result);
?>
--EXPECT--
array(1) {
  [0]=>
  array(1) {
    ["@key"]=>
    int(42)
  }
}
