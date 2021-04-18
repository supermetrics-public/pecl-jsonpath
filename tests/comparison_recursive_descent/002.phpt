--TEST--
Test recursive descent after dot notation
--SKIPIF--
<?php if (!extension_loaded("jsonpath")) print "skip"; ?>
--FILE--
<?php

$data = [
    "some key" => "value",
    "key" => [
        "complex" => "string",
        "primitives" => [
            0,
            1,
        ],
    ],
];

$jsonPath = new JsonPath();
$result = $jsonPath->find($data, "$.key..");

echo "Assertion 1\n";
var_dump($result);
?>
--EXPECT--
PHP Fatal Error
--XFAIL--
Now returns false, would be better to error out due to invalid syntax
