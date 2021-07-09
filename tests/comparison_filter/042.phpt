--TEST--
Test filter expression with equals string with current object literal
--SKIPIF--
<?php if (!extension_loaded("jsonpath")) print "skip"; ?>
--FILE--
<?php

$data = [
    [
        "key" => "some",
    ],
    [
        "key" => "value",
    ],
    [
        "key" => "hi@example.com",
    ],
];

$jsonPath = new \JsonPath\JsonPath();
$result = $jsonPath->find($data, "$[?(@.key==\"hi@example.com\")]");

var_dump($result);
?>
--EXPECT--
array(1) {
  [0]=>
  array(1) {
    ["key"]=>
    string(14) "hi@example.com"
  }
}
