--TEST--
Test dot bracket notation with double quotes
--SKIPIF--
<?php if (!extension_loaded("jsonpath")) print "skip"; ?>
--FILE--
<?php

$data = [
    "key" => "value",
    "other" => [
        "key" => [
            "key" => 42,
        ],
    ],
];

$jsonPath = new \JsonPath\JsonPath();
$result = $jsonPath->find($data, '$.["key"]');

var_dump($result);
?>
--EXPECT--
array(1) {
  [0]=>
  string(5) "value"
}
