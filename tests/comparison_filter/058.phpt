--TEST--
Test filter expression with regular expression from member
--SKIPIF--
<?php if (!extension_loaded("jsonpath")) print "skip"; ?>
--Description--
JSONPath node selectors are not interpolated within regex patterns.
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
    [
        "pattern" => "hello.*",
    ],
];

$jsonPath = new JsonPath();
$result = $jsonPath->find($data, "$[?(@.name=~/@.pattern/)]");

var_dump($result);
?>
--EXPECT--
bool(false)