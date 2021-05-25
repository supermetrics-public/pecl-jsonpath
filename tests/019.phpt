--TEST--
Test broken boolean as right-hand operand
--SKIPIF--
<?php if (!extension_loaded("jsonpath")) print "skip"; ?>
--FILE--
<?php

$jsonPath = new JsonPath();

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

$jsonPath = new JsonPath();
$result = $jsonPath->find($data, "$.words[?(@.isAdjective == truse)]");

echo "Assertion 1\n";
var_dump($result);
?>
--EXPECTF--
Fatal error: Uncaught RuntimeException: Expected `true` or `false` for boolean token in %s
Stack trace:
%s
%s
%s
