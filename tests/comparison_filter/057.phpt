--TEST--
Test filter expression with regular expression
--SKIPIF--
<?php if (!extension_loaded("jsonpath")) print "skip"; ?>
--INI--
pcre.jit=0
--FILE--
<?php

$data = [
    [
        "name" => "hullo world",
    ],
    [
        "name" => "hello world",
    ],
    [
        "name" => "yes hello world",
    ],
    [
        "name" => "HELLO WORLD",
    ],
    [
        "name" => "good bye",
    ],
];

$jsonPath = new JsonPath();
$result = $jsonPath->find($data, "$[?(@.name=~/hello.*/)]");

var_dump($result);
?>
--EXPECT--
array(2) {
  [0]=>
  array(1) {
    ["name"]=>
    string(11) "hello world"
  }
  [1]=>
  array(1) {
    ["name"]=>
    string(15) "yes hello world"
  }
}