--TEST--
Test broken boolean as left-hand operand
--SKIPIF--
<?php if (!extension_loaded("jsonpath")) print "skip"; ?>
--FILE--
<?php

$jsonPath = new \JsonPath\JsonPath();

$data = [
  'words' => [
     [
      'isAdjective' => true,
      'value'       => 'soggy',
    ],
    [
      'isAdjective' => true,
      'value'       => 'minor',
    ],
    [
      'isAdjective' => true,
      'value'       => 'elated',
    ],
    [
      'isAdjective' => true,
      'value'       => 'unnatural',
    ],
    [
      'isAdjective' => true,
      'value'       => 'grubby',
    ],
  ],
];

$jsonPath = new \JsonPath\JsonPath();
$result = $jsonPath->find($data, "$.words[?(falsue == @.isAdjective)]");

var_dump($result);
?>
--EXPECTF--
Fatal error: Uncaught JsonPath\JsonPathException: Expected `true` or `false` for boolean token in %s
Stack trace:
%s
%s
%s
