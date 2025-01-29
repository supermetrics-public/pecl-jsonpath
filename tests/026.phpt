--TEST--
Test greater-than number comparison condition with float compared to numbers and numerical strings
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
$result = $jsonPath->find($data, "$[?(@.quantity > 0.2)]");

var_dump($result);
?>
--EXPECT--
array(5) {
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
    string(7) "coconut"
    ["quantity"]=>
    string(3) "0.5"
  }
  [4]=>
  array(2) {
    ["product"]=>
    string(6) "carrot"
    ["quantity"]=>
    string(1) "3"
  }
}