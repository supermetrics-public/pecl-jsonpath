--TEST--
Test greater-than-or-equal-to number comparison condition with integer compared to numbers and numerical strings
--SKIPIF--
<?php if (!extension_loaded("jsonpath")) print "skip"; ?>
--FILE--
<?php

$data = [
    [
        'product'  => 'tomato',
        'quantity' => '4',
    ],
    [
        'product'  => 'apple',
        'quantity' => '1',
    ],
    [
        'product'  => 'pear',
        'quantity' => 2,
    ],
    [
        'product'  => 'banana',
        'quantity' => '0',
    ],
    [
        'product'  => 'cucumber',
        'quantity' => 0,
    ],
    [
        'product'  => 'coconut',
        'quantity' => '0.5',
    ],
    [
        'product'  => 'carrot',
        'quantity' => '3',
    ],
    [
        'product'  => 'cauliflower',
        'quantity' => '-4',
    ],
    [
        'product'  => 'watermelon',
        'quantity' => '-2',
    ],
    [
        'product'  => 'asparagus',
        'quantity' => '-2.5',
    ],
];

$jsonPath = new \JsonPath\JsonPath();
$result = $jsonPath->find($data, "$[?(@.quantity >= -3)]");

var_dump($result);
?>
--EXPECT--
array(9) {
  [0]=>
  array(2) {
    ["product"]=>
    string(6) "tomato"
    ["quantity"]=>
    string(1) "4"
  }
  [1]=>
  array(2) {
    ["product"]=>
    string(5) "apple"
    ["quantity"]=>
    string(1) "1"
  }
  [2]=>
  array(2) {
    ["product"]=>
    string(4) "pear"
    ["quantity"]=>
    int(2)
  }
  [3]=>
  array(2) {
    ["product"]=>
    string(6) "banana"
    ["quantity"]=>
    string(1) "0"
  }
  [4]=>
  array(2) {
    ["product"]=>
    string(8) "cucumber"
    ["quantity"]=>
    int(0)
  }
  [5]=>
  array(2) {
    ["product"]=>
    string(7) "coconut"
    ["quantity"]=>
    string(3) "0.5"
  }
  [6]=>
  array(2) {
    ["product"]=>
    string(6) "carrot"
    ["quantity"]=>
    string(1) "3"
  }
  [7]=>
  array(2) {
    ["product"]=>
    string(10) "watermelon"
    ["quantity"]=>
    string(2) "-2"
  }
  [8]=>
  array(2) {
    ["product"]=>
    string(9) "asparagus"
    ["quantity"]=>
    string(4) "-2.5"
  }
}