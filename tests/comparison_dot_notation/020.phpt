--TEST--
Test dot notation after union with keys
--SKIPIF--
<?php if (!extension_loaded("jsonpath")) print "skip"; ?>
--FILE--
<?php

$data = [
    "one" => [
        "key" => "value",
    ],
    "two" => [
        "k" => "v",
    ],
    "three" => [
        "some" => "more",
        "key" => "other value",
    ],
];

$jsonPath = new JsonPath();
$result = $jsonPath->find($data, "$['one','three'].key");

echo "Assertion 1\n";
var_dump($result);
?>
--EXPECT--
Assertion 1
array(2) {
  [0]=>
  string(5) "value"
  [1]=>
  string(11) "other value"
}
--XFAIL--
Requires support for union with string literal keys
