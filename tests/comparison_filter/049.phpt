--TEST--
Test filter expression with in array of values
--SKIPIF--
<?php if (!extension_loaded("jsonpath")) print "skip"; ?>
--FILE--
<?php

$data = [
    [
        "d" => 1,
    ],
    [
        "d" => 2,
    ],
    [
        "d" => 1,
    ],
    [
        "d" => 3,
    ],
    [
        "d" => 4,
    ],
];

$jsonPath = new JsonPath();
$result = $jsonPath->find($data, "$[?(@.d in [2, 3])]");

echo "Assertion 1\n";
var_dump($result);
?>
--EXPECT--
PHP Fatal Error
--XFAIL--
Should error out due to invalid syntax, now get_token_type: Assertion `0' failed
