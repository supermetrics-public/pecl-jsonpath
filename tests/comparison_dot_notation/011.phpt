--TEST--
Test dot notation after array slice
--SKIPIF--
<?php if (!extension_loaded("jsonpath")) print "skip"; ?>
--FILE--
<?php

$data = [
    [
        "key" => "ey",
    ],
    [
        "key" => "bee",
    ],
    [
        "key" => "see",
    ],
];

$jsonPath = new \JsonPath\JsonPath();
$result = $jsonPath->find($data, "$[0:2].key");

var_dump($result);
?>
--EXPECT--
array(2) {
  [0]=>
  string(2) "ey"
  [1]=>
  string(3) "bee"
}
