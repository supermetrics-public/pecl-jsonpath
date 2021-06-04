--TEST--
Test filter with overflowing operand
--SKIPIF--
<?php if (!extension_loaded("jsonpath")) print "skip"; ?>
--FILE--
<?php

$jsonPath = new JsonPath();

$data = [
  'words' => [
     [
      'occurrences' => 647812,
      'value'       => 'soggy',
    ],
    [
      'occurrences' => 214216781,
      'value'       => 'minor',
    ],
    [
      'occurrences' => 1426,
      'value'       => 'elated',
    ],
    [
      'occurrences' => 126781,
      'value'       => 'unnatural',
    ],
    [
      'occurrences' => 972840012,
      'value'       => 'grubby',
    ],
  ],
];

$jsonPath = new JsonPath();
$result = $jsonPath->find($data, "$.words[?($.occurrences > 9223372036854775808)]");

var_dump($result);
?>
--EXPECTF--
Fatal error: Uncaught RuntimeException: Unable to parse numeric in %s
Stack trace:
%s
%s
%s
