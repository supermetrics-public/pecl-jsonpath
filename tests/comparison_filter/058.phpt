--TEST--
Test filter expression with regular expression from member
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
    [
        "pattern" => "hello.*",
    ],
];

$jsonPath = new JsonPath();
$result = $jsonPath->find($data, "$[?(@.name=~/@.pattern/)]");

echo "Assertion 1\n";
var_dump($result);
?>
--EXPECTF--
Fatal error: Uncaught RuntimeException: Unrecognized token `/` at position 12 in %s
Stack trace:
%s
%s
%s