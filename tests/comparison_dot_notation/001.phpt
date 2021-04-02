--TEST--
Test dot bracket notation
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

$jsonPath = new JsonPath();
$result = $jsonPath->find($data, "$.['key']");

echo "Assertion 1\n";
var_dump($result);
?>
--EXPECT--
Assertion 1
array(1) {
  [0]=>
  string(5) "value"
}
