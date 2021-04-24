--TEST--
Test filter expression with regular expression
--SKIPIF--
<?php if (!extension_loaded("jsonpath")) print "skip"; ?>
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

echo "Assertion 1\n";
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
    string(14) "yes hello world"
  }
}
--XFAIL--
Regexps currently need to be wrapped in quotes
