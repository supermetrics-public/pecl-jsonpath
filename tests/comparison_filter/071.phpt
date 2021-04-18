--TEST--
Test filter expression with value from recursive descent
--SKIPIF--
<?php if (!extension_loaded("jsonpath")) print "skip"; ?>
--FILE--
<?php

$data = [
    [
        "key" => [
            [
                "child" => 1,
            ],
            [
                "child" => 2,
            ],
        ],
    ],
    [
        "key" => [
            [
                "child" => 2,
            ],
        ],
    ],
    [
        "key" => [
            [],
        ],
    ],
    [
        "key" => [
            [
                "something" => 42,
            ],
        ],
    ],
    [],
];

$jsonPath = new JsonPath();
$result = $jsonPath->find($data, "$[?(@..child)]");

echo "Assertion 1\n";
var_dump($result);
?>
--EXPECT--
??
--XFAIL--
Not sure what would be the best outcome here
