--TEST--
Test array slice with wildcard as end
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
$result = $jsonPath->find($data, "$.words[0:*:2]");

echo "Assertion 1\n";
var_dump($result);
?>
--EXPECTF--
Fatal error: Uncaught RuntimeException: Unrecognized token `*` at position 10 in %s
Stack trace:
%s
%s
%s
