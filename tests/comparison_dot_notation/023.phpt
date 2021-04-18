--TEST--
Test dot notation with double quotes after recursive descent
--SKIPIF--
<?php if (!extension_loaded("jsonpath")) print "skip"; ?>
--FILE--
<?php

$data = [
    "object" => [
        "key" => "value",
        "\"key\"" => 42,
        "array" => [
            [
                "key" => "something",
                "\"key\"" => 0,
            ],
            [
                "key" => [
                    "key" => "russian dolls",
                ],
                "\"key\"" => [
                    "\"key\"" => 99,
                ],
            ],
        ],
    ],
    "key" => "top",
    "\"key\"" => 42,
];

$jsonPath = new JsonPath();
$result = $jsonPath->find($data, '$.."key"');

echo "Assertion 1\n";
var_dump($result);
?>
--EXPECT--
PHP Fatal Error
--XFAIL--
Now returns false, would be better to error out due to invalid syntax
