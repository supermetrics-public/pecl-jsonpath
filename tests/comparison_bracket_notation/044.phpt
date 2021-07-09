--TEST--
Test bracket notation with wildcard after dot notation after bracket notation with wildcard
--SKIPIF--
<?php if (!extension_loaded("jsonpath")) print "skip"; ?>
--FILE--
<?php

$data = [
    [
        "bar" => [
            42,
        ],
    ],
];

$jsonPath = new \JsonPath\JsonPath();
$result = $jsonPath->find($data, "$[*].bar[*]");

var_dump($result);
?>
--EXPECT--
array(1) {
  [0]=>
  int(42)
}
