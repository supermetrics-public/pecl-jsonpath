--TEST--
Test filter expression with regular expression that contains escaped slash
--SKIPIF--
<?php if (!extension_loaded("jsonpath")) print "skip"; ?>
--FILE--
<?php

$data = [
    [
        "name" => "hello world",
    ],
    [
        "name" => "yes hello/ world",
    ],
    [
        "name" => "HELLO WORLD",
    ],
    [
        "name" => "good bye",
    ],
];

$jsonPath = new \JsonPath\JsonPath();
$result = $jsonPath->find($data, "$[?(@.name=~/hello\/.*/)]");

var_dump($result);
?>
--EXPECT--
array(1) {
  [0]=>
  array(1) {
    ["name"]=>
    string(16) "yes hello/ world"
  }
}