--TEST--
Test dot notation after bracket notation after recursive descent
--SKIPIF--
<?php if (!extension_loaded("jsonpath")) print "skip"; ?>
--FILE--
<?php

$data = [
    "k" => [
        [
            "key" => "some value",
        ],
        [
            "key" => 42,
        ],
    ],
    "kk" => [
        [
            [
                "key" => 100,
            ],
            [
                "key" => 200,
            ],
            [
                "key" => 300,
            ],
            [
                "key" => 400,
            ],
            [
                "key" => 500,
            ],
            [
                "key" => 600,
            ],
        ],
    ],
    "key" => [
        0,
        1,
    ],
];

$jsonPath = new JsonPath();
$result = $jsonPath->find($data, "$..[1].key");

echo "Assertion 1\n";
var_dump($result);
?>
--EXPECT--
Assertion 1
array(3) {
  [0]=>
  int(200)
  [1]=>
  int(42)
  [2]=>
  int(500)
}
--XFAIL--
Requires more work on the recursive descent implementation
