--TEST--
Test filter expression with equals boolean expression value
--SKIPIF--
<?php if (!extension_loaded("jsonpath")) print "skip"; ?>
--FILE--
<?php

$data = [
    [
        "key" => 42,
    ],
    [
        "key" => 43,
    ],
    [
        "key" => 44,
    ],
];

$jsonPath = new \JsonPath\JsonPath();
$result = $jsonPath->find($data, "$[?((@.key<44)==false)]");

var_dump($result);
?>
--EXPECT--
array(1) {
  [0]=>
  array(1) {
    ["key"]=>
    int(44)
  }
}