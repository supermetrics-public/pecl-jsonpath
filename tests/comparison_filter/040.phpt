--TEST--
Test filter expression with equals object
--SKIPIF--
<?php if (!extension_loaded("jsonpath")) print "skip"; ?>
--FILE--
<?php

$data = [
    [
        "d" => [
            "k" => "v",
        ],
    ],
    [
        "d" => [
            "a" => "b",
        ],
    ],
    [
        "d" => "k",
    ],
    [
        "d" => "v",
    ],
    [
        "d" => [
        ],
    ],
    [
        "d" => [
        ],
    ],
    [
        "d" => null,
    ],
    [
        "d" => -1,
    ],
    [
        "d" => 0,
    ],
    [
        "d" => 1,
    ],
    [
        "d" => "[object Object]",
    ],
    [
        "d" => "{\"k\": \"v\"}",
    ],
    [
        "d" => "{\"k\":\"v\"}",
    ],
    "v",
];

$jsonPath = new JsonPath();
$result = $jsonPath->find($data, "$[?(@.d=={"k":"v"})]");

echo "Assertion 1\n";
var_dump($result);
?>
--EXPECT--
Assertion 1
array(1) {
  [0]=>
  array(1) {
    ["d"]=>
    string(9) "{"k":"v"}"
  }
}
--XFAIL--
Requires more work on filter expressions
