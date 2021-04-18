--TEST--
Test filter expression with set wise comparison to set
--SKIPIF--
<?php if (!extension_loaded("jsonpath")) print "skip"; ?>
--FILE--
<?php

$data = [
    "x" => [
        [
            1,
            2,
        ],
        [
            3,
            4,
        ],
        [
            5,
            6,
        ],
    ],
    "y" => [
        3,
        4,
        5,
    ],
];

$jsonPath = new JsonPath();
$result = $jsonPath->find($data, "$.x[?(@[*]>=$.y[*])]");

echo "Assertion 1\n";
var_dump($result);
?>
--EXPECT--
PHP Fatal Error
--XFAIL--
Should error out due to invalid syntax, now get_token_type: Assertion `0' failed
