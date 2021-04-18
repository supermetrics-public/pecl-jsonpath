--TEST--
Test filter expression with subfilter
--SKIPIF--
<?php if (!extension_loaded("jsonpath")) print "skip"; ?>
--FILE--
<?php

$data = [
    [
        "a" => [
            [
                "price" => 1,
            ],
            [
                "price" => 3,
            ],
        ],
    ],
    [
        "a" => [
            [
                "price" => 11,
            ],
        ],
    ],
    [
        "a" => [
            [
                "price" => 8,
            ],
            [
                "price" => 12,
            ],
            [
                "price" => 3,
            ],
        ],
    ],
    [
        "a" => []
    ],
];

$jsonPath = new JsonPath();
$result = $jsonPath->find($data, "$[?(@.a[?(@.price>10)])]");

echo "Assertion 1\n";
var_dump($result);
?>
--EXPECT--
PHP Fatal Error
--XFAIL--
Should error out due to invalid syntax, now get_token_type: Assertion `0' failed
