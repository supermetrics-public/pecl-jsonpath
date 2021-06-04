--TEST--
Test bracket notation with spaces
--SKIPIF--
<?php if (!extension_loaded("jsonpath")) print "skip"; ?>
--FILE--
<?php

$data = [
    " a" => 1,
    "a" => 2,
    " a " => 3,
    "a " => 4,
    " 'a' " => 5,
    " 'a" => 6,
    "a' " => 7,
    " \"a\" " => 8,
    "\"a\"" => 9,
];

$jsonPath = new JsonPath();
$result = $jsonPath->find($data, "$[ 'a' ]");

var_dump($result);
?>
--EXPECT--
array(1) {
  [0]=>
  int(2)
}
