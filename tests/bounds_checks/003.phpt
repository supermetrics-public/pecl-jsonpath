--TEST--
Test array slice with overflowing values for start, end, and stop
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
$result = $jsonPath->find($data, "$.words[9223372036854775808:9223372036854775809:9223372036854775810]");

var_dump($result);
?>
--EXPECTF--
Fatal error: Uncaught RuntimeException: Unable to parse filter index value in %s
Stack trace:
%s
%s
%s
