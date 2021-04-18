--TEST--
Test union with repeated matches after dot notation with wildcard
--SKIPIF--
<?php if (!extension_loaded("jsonpath")) print "skip"; ?>
--FILE--
<?php

$data = [
    "a" => [
        "string",
        null,
        true,
    ],
    "b" => [
        false,
        "string",
        5.4,
    ],
];

$jsonPath = new JsonPath();
$result = $jsonPath->find($data, "$.*[0,:5]");

echo "Assertion 1\n";
var_dump($result);
?>
--EXPECT--
PHP Fatal Error
--XFAIL--
Now returns a bunch of values, would be better to error out due to invalid syntax
