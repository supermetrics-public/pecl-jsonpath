--TEST--
Test union with keys after dot notation with wildcard
--SKIPIF--
<?php if (!extension_loaded("jsonpath")) print "skip"; ?>
--FILE--
<?php

$data = [
    [
        "c" => "cc1",
        "d" => "dd1",
        "e" => "ee1",
    ],
    [
        "c" => "cc2",
        "d" => "dd2",
        "e" => "ee2",
    ],
];

$jsonPath = new \JsonPath\JsonPath();
$result = $jsonPath->find($data, "$.*['c','d']");

var_dump($result);
?>
--EXPECT--
array(4) {
  [0]=>
  string(3) "cc1"
  [1]=>
  string(3) "dd1"
  [2]=>
  string(3) "cc2"
  [3]=>
  string(3) "dd2"
}