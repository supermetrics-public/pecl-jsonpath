--TEST--
Test bracket notation after recursive descent
--SKIPIF--
<?php if (!extension_loaded("jsonpath")) print "skip"; ?>
--FILE--
<?php

$data = [
    "first",
    [
        "key" => [
            "first nested",
            [
                "more" => [
                    [
                        "nested" => [
                            "deepest",
                            "second",
                        ],
                    ],
                    [
                        "more",
                        "values",
                    ],
                ],
            ],
        ],
    ],
];

$jsonPath = new JsonPath();
$result = $jsonPath->find($data, '$..[0]');

echo "Assertion 1\n";
var_dump($result);
?>
--EXPECT--
Assertion 1
array(5) {
  [0]=>
  string(7) "deepest"
  [1]=>
  string(12) "first nested"
  [2]=>
  string(5) "first"
  [3]=>
  string(4) "more"
  [4]=>
  array(1) {
    ["nested"]=>
    array(2) {
      [0]=>
      string(7) "deepest"
      [1]=>
      string(6) "second"
    }
  }
}
--XFAIL--
Requires more work on the recursive descent implementation
