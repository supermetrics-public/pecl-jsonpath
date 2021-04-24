--TEST--
Test filter expression with not equals
--SKIPIF--
<?php if (!extension_loaded("jsonpath")) print "skip"; ?>
--FILE--
<?php

$data = [
    [
        "key" => 0,
    ],
    [
        "key" => 42,
    ],
    [
        "key" => -1,
    ],
    [
        "key" => 1,
    ],
    [
        "key" => 41,
    ],
    [
        "key" => 43,
    ],
    [
        "key" => 42.0001,
    ],
    [
        "key" => 41.9999,
    ],
    [
        "key" => 100,
    ],
    [
        "key" => "some",
    ],
    [
        "key" => "42",
    ],
    [
        "key" => null,
    ],
    [
        "key" => 420,
    ],
    [
        "key" => "",
    ],
    [
        "key" => [],
    ],
    [
        "key" => [],
    ],
    [
        "key" => [
            42,
        ],
    ],
    [
        "key" => [
            "key" => 42,
        ],
    ],
    [
        "key" => [
            "some" => 42,
        ],
    ],
    [
        "some" => "value",
    ],
];

$jsonPath = new JsonPath();
$result = $jsonPath->find($data, "$[?(@.key!=42)]");

echo "Assertion 1\n";
var_dump($result);
?>
--EXPECT--
array(19) {
  [0]=>
  array(1) {
    ["key"]=>
    int(0)
  }
  [1]=>
  array(1) {
    ["key"]=>
    int(-1)
  }
  [2]=>
  array(1) {
    ["key"]=>
    int(1)
  }
  [3]=>
  array(1) {
    ["key"]=>
    int(41)
  }
  [4]=>
  array(1) {
    ["key"]=>
    int(43)
  }
  [5]=>
  array(1) {
    ["key"]=>
    float(42.0001)
  }
  [6]=>
  array(1) {
    ["key"]=>
    float(41.9999)
  }
  [7]=>
  array(1) {
    ["key"]=>
    int(100)
  }
  [8]=>
  array(1) {
    ["key"]=>
    string(4) "some"
  }
  [9]=>
  array(1) {
    ["key"]=>
    string(2) "42"
  }
  [10]=>
  array(1) {
    ["key"]=>
    NULL
  }
  [11]=>
  array(1) {
    ["key"]=>
    int(420)
  }
  [12]=>
  array(1) {
    ["key"]=>
    string(0) ""
  }
  [13]=>
  array(1) {
    ["key"]=>
    array(0) {
    }
  }
  [14]=>
  array(1) {
    ["key"]=>
    array(0) {
    }
  }
  [15]=>
  array(1) {
    ["key"]=>
    array(1) {
      [0]=>
      int(42)
    }
  }
  [16]=>
  array(1) {
    ["key"]=>
    array(1) {
      ["key"]=>
      int(42)
    }
  }
  [17]=>
  array(1) {
    ["key"]=>
    array(1) {
      ["some"]=>
      int(42)
    }
  }
  [18]=>
  array(1) {
    ["some"]=>
    string(5) "value"
  }
}
--XFAIL--
Outcome depends on how values of different types are compared (string "42"), fails to include item with missing `key` property
