--TEST--
Test union with keys after bracket notation
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

$jsonPath = new JsonPath();
$result = $jsonPath->find($data, "$[0]['c','d']");

var_dump($result);
?>
--EXPECT--
array(2) {
  [0]=>
  string(3) "cc1"
  [1]=>
  string(3) "dd1"
}