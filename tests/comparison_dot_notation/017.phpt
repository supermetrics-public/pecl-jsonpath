--TEST--
Test dot notation after recursive descent
--SKIPIF--
<?php if (!extension_loaded("jsonpath")) print "skip"; ?>
--FILE--
<?php
require_once __DIR__ . '/../utils/sort_recursively.php';

$data = [
    "object" => [
        "key" => "value",
        "array" => [
            [
                "key" => "something",
            ],
            [
                "key" => [
                    "key" => "russian dolls",
                ],
            ],
        ],
    ],
    "key" => "top",
];

$jsonPath = new JsonPath();
$result = $jsonPath->find($data, "$..key");
sortRecursively($result);

echo "Assertion 1\n";
var_dump($result);
?>
--EXPECT--
Assertion 1
array(5) {
  [0]=>
  array(1) {
    ["key"]=>
    string(13) "russian dolls"
  }
  [1]=>
  string(13) "russian dolls"
  [2]=>
  string(9) "something"
  [3]=>
  string(3) "top"
  [4]=>
  string(5) "value"
}
