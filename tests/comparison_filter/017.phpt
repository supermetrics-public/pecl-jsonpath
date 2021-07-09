--TEST--
Test filter expression with different grouped operators
--SKIPIF--
<?php if (!extension_loaded("jsonpath")) print "skip"; ?>
--FILE--
<?php

$data = [
    [
        "a" => true,
    ],
    [
        "a" => true,
        "b" => true,
    ],
    [
        "a" => true,
        "b" => true,
        "c" => true,
    ],
    [
        "b" => true,
        "c" => true,
    ],
    [
        "a" => true,
        "c" => true,
    ],
    [
        "c" => true,
    ],
    [
        "b" => true,
    ],
];

$jsonPath = new \JsonPath\JsonPath();
$result = $jsonPath->find($data, "$[?(@.a && (@.b || @.c))]");

var_dump($result);
?>
--EXPECT--
array(3) {
  [0]=>
  array(2) {
    ["a"]=>
    bool(true)
    ["b"]=>
    bool(true)
  }
  [1]=>
  array(3) {
    ["a"]=>
    bool(true)
    ["b"]=>
    bool(true)
    ["c"]=>
    bool(true)
  }
  [2]=>
  array(2) {
    ["a"]=>
    bool(true)
    ["c"]=>
    bool(true)
  }
}
