--TEST--
Test array slice with unquoted strings as start, end, and step
--SKIPIF--
<?php if (!extension_loaded("jsonpath")) print "skip"; ?>
--FILE--
<?php

$jsonPath = new JsonPath();

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

$jsonPath = new JsonPath();
$result = $jsonPath->find($data, "$.words[first:last:two]");

echo "Assertion 1\n";
var_dump($result);
?>
--EXPECTF--
Fatal error: Uncaught RuntimeException: Unrecognized token `l` at position 14 in %s
Stack trace:
%s
%s
%s
