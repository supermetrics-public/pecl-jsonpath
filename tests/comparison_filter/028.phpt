--TEST--
Test filter expression with equals array
--SKIPIF--
<?php if (!extension_loaded("jsonpath")) print "skip"; ?>
--FILE--
<?php

$data = [
    [
        "d" => [
            "v1",
            "v2",
        ],
    ],
    [
        "d" => [
            "a",
            "b",
        ],
    ],
    [
        "d" => "v1",
    ],
    [
        "d" => "v2",
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
        "d" => "['v1','v2']",
    ],
    [
        "d" => "['v1', 'v2']",
    ],
    [
        "d" => "v1,v2",
    ],
    [
        "d" => "[\"v1\", \"v2\"]",
    ],
    [
        "d" => "[\"v1\",\"v2\"]",
    ],
];

$jsonPath = new JsonPath();
$result = $jsonPath->find($data, '$[?(@.d==["v1","v2"])]');

echo "Assertion 1\n";
var_dump($result);
?>
--EXPECT--
Assertion 1
array(1) {
  [0]=>
  array(1) {
    ["d"]=>
    array(2) {
      [0]=>
      string(2) "v1"
      [1]=>
      string(2) "v2"
    }
  }
}