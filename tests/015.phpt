--TEST--
Test array slice with strings as start, end, and step
--SKIPIF--
<?php if (!extension_loaded("jsonpath")) print "skip"; ?>
--FILE--
<?php

$jsonPath = new \JsonPath\JsonPath();

$data = [
  'words' => [
    'soggy',
    'minor',
    'elated',
    'unnatural',
    'grubby',
    'fabulous',
    'parsimonious',
    'cheerful',
    'unadvised',
    'simplistic',
  ],
];

$jsonPath = new \JsonPath\JsonPath();
$result = $jsonPath->find($data, "$.words['first':'last':'two']");

var_dump($result);
?>
--EXPECTF--
Fatal error: Uncaught JsonPath\JsonPathException: Expected slice separator `:` or integer, got LEX_LITERAL in slice union filter in %s
Stack trace:
%s
%s
%s
