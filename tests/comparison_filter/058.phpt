--TEST--
Test filter expression with regular expression from member
--SKIPIF--
<?php if (!extension_loaded("jsonpath")) print "skip"; ?>
--DESCRIPTION--
JSONPath node selectors are not interpolated within regex patterns.
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
    [
        "pattern" => "hello.*",
    ],
];

$jsonPath = new \JsonPath\JsonPath();
$result = $jsonPath->find($data, "$[?(@.name=~/@.pattern/)]");

var_dump($result);
?>
--EXPECT--
bool(false)