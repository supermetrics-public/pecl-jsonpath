--TEST--
Test filter expression with set wise comparison to scalar
--SKIPIF--
<?php if (!extension_loaded("jsonpath")) print "skip"; ?>
--FILE--
<?php

$data = [
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
];

$jsonPath = new JsonPath();
$result = $jsonPath->find($data, "$[?(@[*]>=4)]");

echo "Assertion 1\n";
var_dump($result);
?>
--EXPECT--
PHP Fatal Error
--XFAIL--
Should error out due to invalid syntax, now get_token_type: Assertion `0' failed
