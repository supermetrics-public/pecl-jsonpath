--TEST--
Test filter expression with subfilter
--SKIPIF--
<?php if (!extension_loaded("jsonpath")) print "skip"; ?>
--FILE--
<?php

$data = [
    [
        "a" => [
            [
                "price" => 1,
            ],
            [
                "price" => 3,
            ],
        ],
    ],
    [
        "a" => [
            [
                "price" => 11,
            ],
        ],
    ],
    [
        "a" => [
            [
                "price" => 8,
            ],
            [
                "price" => 12,
            ],
            [
                "price" => 3,
            ],
        ],
    ],
    [
        "a" => []
    ],
];

$jsonPath = new JsonPath();
$result = $jsonPath->find($data, "$[?(@.a[?(@.price>10)])]");

var_dump($result);
?>
--EXPECT--
array(2) {
  [0]=>
  array(1) {
    ["a"]=>
    array(1) {
      [0]=>
      array(1) {
        ["price"]=>
        int(11)
      }
    }
  }
  [1]=>
  array(1) {
    ["a"]=>
    array(3) {
      [0]=>
      array(1) {
        ["price"]=>
        int(8)
      }
      [1]=>
      array(1) {
        ["price"]=>
        int(12)
      }
      [2]=>
      array(1) {
        ["price"]=>
        int(3)
      }
    }
  }
}
