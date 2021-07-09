--TEST--
Test filter expression with equals string
--SKIPIF--
<?php if (!extension_loaded("jsonpath")) print "skip"; ?>
--FILE--
<?php

$data = [
    [
        "key" => "some",
    ],
    [
        "key" => "value",
    ],
    [
        "key" => null,
    ],
    [
        "key" => 0,
    ],
    [
        "key" => 1,
    ],
    [
        "key" => -1,
    ],
    [
        "key" => "",
    ],
    [
        "key" => [],
    ],
    [
        "key" => [],
    ],
    [
        "key" => "valuemore",
    ],
    [
        "key" => "morevalue",
    ],
    [
        "key" => [
            "value",
        ],
    ],
    [
        "key" => [
            "some" => "value",
        ],
    ],
    [
        "key" => [
            "key" => "value",
        ],
    ],
    [
        "some" => "value",
    ],
];

$jsonPath = new \JsonPath\JsonPath();
$result = $jsonPath->find($data, "$[?(@.key==\"value\")]");

var_dump($result);
?>
--EXPECT--
array(1) {
  [0]=>
  array(1) {
    ["key"]=>
    string(5) "value"
  }
}