--TEST--
Test dot notation after filter expression
--SKIPIF--
<?php if (!extension_loaded("jsonpath")) print "skip"; ?>
--FILE--
<?php

$data = [
    [
        "id" => 42,
        "name" => "forty-two",
    ],
    [
        "id" => 1,
        "name" => "one",
    ],
];

$jsonPath = new JsonPath();
$result = $jsonPath->find($data, "$[?(@.id==42)].name");

echo "Assertion 1\n";
var_dump($result);
?>
--EXPECT--
Assertion 1
array(1) {
  [0]=>
  string(9) "forty-two"
}
