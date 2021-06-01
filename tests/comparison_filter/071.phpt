--TEST--
Test filter expression with value from recursive descent
--SKIPIF--
<?php if (!extension_loaded("jsonpath")) print "skip"; ?>
--FILE--
<?php

$data = [
    [
        "key" => [
            [
                "child" => 1,
            ],
            [
                "child" => 2,
            ],
        ],
    ],
    [
        "key" => [
            [
                "child" => 2,
            ],
        ],
    ],
    [
        "key" => [
            [],
        ],
    ],
    [
        "key" => [
            [
                "something" => 42,
            ],
        ],
    ],
    [],
];

$jsonPath = new JsonPath();
$result = $jsonPath->find($data, "$[?(@..child)]");

var_dump($result);
?>
--EXPECT--
array(2) {
  [0]=>
  array(1) {
    ["key"]=>
    array(2) {
      [0]=>
      array(1) {
        ["child"]=>
        int(1)
      }
      [1]=>
      array(1) {
        ["child"]=>
        int(2)
      }
    }
  }
  [1]=>
  array(1) {
    ["key"]=>
    array(1) {
      [0]=>
      array(1) {
        ["child"]=>
        int(2)
      }
    }
  }
}