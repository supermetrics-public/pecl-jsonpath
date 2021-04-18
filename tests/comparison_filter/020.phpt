--TEST--
Test filter expression with dot notation with dash
--SKIPIF--
<?php if (!extension_loaded("jsonpath")) print "skip"; ?>
--FILE--
<?php

$data = [
    [
        "key-dash" => "value",
    ],
];

$jsonPath = new JsonPath();
$result = $jsonPath->find($data, "$[?(@.key-dash == 'value')]");

echo "Assertion 1\n";
var_dump($result);
?>
--EXPECT--
Assertion 1
array(1) {
  [0]=>
  array(1) {
    ["key-dash"]=>
    string(5) "value"
  }
}
