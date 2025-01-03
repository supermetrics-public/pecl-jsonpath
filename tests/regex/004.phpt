--TEST--
Test regex comparison against non-string values
--SKIPIF--
<?php if (!extension_loaded("jsonpath")) print "skip"; ?>
--FILE--
<?php

$byRef = 44;

$data = [
  'test' => [
    [
      'id'       => 1,
      'quantity' => 25,
    ],
    [
      'id'       => 2,
      'quantity' => 15,
    ],
    [
      'id'       => 3,
      'quantity' => false,
    ],
    [
      'id'       => 4,
      'quantity' => true,
    ],
    [
      'id'       => 5,
      'quantity' => null,
    ],
    [
      'id'       => 6,
      'quantity' => 107.3,
    ],
    [
      'id'       => 7,
      'quantity' => ['foo'],
    ],
    [
      'id'       => 8,
      'quantity' => new WeakMap(),
    ],
    [
      'id'       => 9,
      'quantity' => &$byRef,
    ],
  ],
];

$jsonPath = new \JsonPath\JsonPath();
$result = $jsonPath->find($data, '$.test[?(@.quantity =~ /^[0-9]+$/)]');

var_dump($result);
--EXPECT--
array(3) {
  [0]=>
  array(2) {
    ["id"]=>
    int(1)
    ["quantity"]=>
    int(25)
  }
  [1]=>
  array(2) {
    ["id"]=>
    int(2)
    ["quantity"]=>
    int(15)
  }
  [2]=>
  array(2) {
    ["id"]=>
    int(9)
    ["quantity"]=>
    &int(44)
  }
}
