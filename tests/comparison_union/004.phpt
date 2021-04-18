--TEST--
Test union with filter
--SKIPIF--
<?php if (!extension_loaded("jsonpath")) print "skip"; ?>
--FILE--
<?php

$data = [
    [
        "key" => 1,
    ],
    [
        "key" => 8,
    ],
    [
        "key" => 3,
    ],
    [
        "key" => 10,
    ],
    [
        "key" => 7,
    ],
    [
        "key" => 2,
    ],
    [
        "key" => 6,
    ],
    [
        "key" => 4,
    ],
];

$jsonPath = new JsonPath();
$result = $jsonPath->find($data, "$[?(@.key<3),?(@.key>6)]");

echo "Assertion 1\n";
var_dump($result);
?>
--EXPECT--
Assertion 1
array(5) {
  [0]=>
  array(1) {
    ["key"]=>
    int(1)
  }
  [1]=>
  array(1) {
    ["key"]=>
    int(2)
  }
  [2]=>
  array(1) {
    ["key"]=>
    int(8)
  }
  [3]=>
  array(1) {
    ["key"]=>
    int(10)
  }
  [4]=>
  array(1) {
    ["key"]=>
    int(7)
  }
}
--XFAIL--
Requires more work on union implementation
