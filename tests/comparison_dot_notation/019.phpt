--TEST--
Test dot notation after union
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

$jsonPath = new JsonPath();
$result = $jsonPath->find($data, "$[0,2].key");

echo "Assertion 1\n";
var_dump($result);
?>
--EXPECT--
Assertion 1
array(2) {
  [0]=>
  string(2) "ey"
  [1]=>
  string(3) "see"
}
