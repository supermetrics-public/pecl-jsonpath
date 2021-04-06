--TEST--
Test dot notation with single quotes and dot
--SKIPIF--
<?php if (!extension_loaded("jsonpath")) print "skip"; ?>
--FILE--
<?php

$data = [
    "some.key" => 42,
    "some" => [
        "key" => "value",
    ],
    "'some.key'" => 43,
];

$jsonPath = new JsonPath();
$result = $jsonPath->find($data, "$.'some.key'");

echo "Assertion 1\n";
var_dump($result);
?>
--EXPECT--
PHP Fatal Error
--XFAIL--
Now returns false, would be better to error out due to invalid syntax
