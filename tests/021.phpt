--TEST--
Test dot selector with misplaced current node operator
--SKIPIF--
<?php if (!extension_loaded("jsonpath")) print "skip"; ?>
--FILE--
<?php

$jsonPath = new JsonPath();

$data = [
  'words' => [
     [
      'type'  => 'adjective',
      'value' => 'soggy',
    ],
    [
      'type'  => 'adjective',
      'value' => 'minor',
    ],
    [
      'type'  => 'adjective',
      'value' => 'elated',
    ],
    [
      'type'  => 'adjective',
      'value' => 'unnatural',
    ],
    [
      'type'  => 'adjective',
      'value' => 'grubby',
    ],
  ],
];

$jsonPath = new JsonPath();
$result = $jsonPath->find($data, "$.words.@");

echo "Assertion 1\n";
var_dump($result);
?>
--EXPECTF--
Fatal error: Uncaught RuntimeException: Expecting child node, filter, expression, or recursive node in %s
Stack trace:
%s
%s
%s
