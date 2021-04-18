--TEST--
Test root
--SKIPIF--
<?php if (!extension_loaded("jsonpath")) print "skip"; ?>
--FILE--
<?php

$data = [
    "key" => "value",
    "another key" => [
        "complex" => [
            "a",
            1,
        ],
    ],
];

$jsonPath = new JsonPath();
$result = $jsonPath->find($data, "$");

echo "Assertion 1\n";
var_dump($result);
?>
--EXPECT--
Assertion 1
array(1) {
  [0]=>
  array(2) {
    ["another key"]=>
    array(1) {
      ["complex"]=>
      array(2) {
        [0]=>
        string(1) "a"
        [1]=>
        int(1)
      }
    }
    ["key"]=>
    string(5) "value"
  }
}
--XFAIL--
This syntax is not currently supported
