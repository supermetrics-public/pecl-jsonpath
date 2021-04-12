--TEST--
Test filter expression with different ungrouped operators
--SKIPIF--
<?php if (!extension_loaded("jsonpath")) print "skip"; ?>
--FILE--
<?php

$data = [
    [
        "a" => true,
        "b" => true,
    ],
    [
        "a" => true,
        "b" => true,
        "c" => true,
    ],
    [
        "b" => true,
        "c" => true,
    ],
    [
        "a" => true,
        "c" => true,
    ],
    [
        "a" => true,
    ],
    [
        "b" => true,
    ],
    [
        "c" => true,
    ],
    [
        "d" => true,
    ],
    [],
];

$jsonPath = new JsonPath();
$result = $jsonPath->find($data, "$[?(@.a && @.b || @.c)]");

echo "Assertion 1\n";
var_dump($result);
?>
--EXPECT--
Assertion 1
array(5) {
  [0]=>
  array(2) {
    ["a"]=>
    bool(true)
    ["b"]=>
    bool(true)
  }
  [1]=>
  array(3) {
    ["a"]=>
    bool(true)
    ["b"]=>
    bool(true)
    ["c"]=>
    bool(true)
  }
  [2]=>
  array(2) {
    ["b"]=>
    bool(true)
    ["c"]=>
    bool(true)
  }
  [3]=>
  array(2) {
    ["a"]=>
    bool(true)
    ["c"]=>
    bool(true)
  }
  [4]=>
  array(1) {
    ["c"]=>
    bool(true)
  }
}

--XFAIL--
Requires more work on filter expressions
