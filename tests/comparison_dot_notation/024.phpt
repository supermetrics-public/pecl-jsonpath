--TEST--
Test dot notation with empty path
--SKIPIF--
<?php if (!extension_loaded("jsonpath")) print "skip"; ?>
--FILE--
<?php

$data = [
    "key" => 42,
    "" => 9001,
    "''" => "nice",
];

$jsonPath = new JsonPath();
$result = $jsonPath->find($data, "$.");

echo "Assertion 1\n";
var_dump($result);
?>
--EXPECT--
PHP Fatal Error
--XFAIL--
Now returns 9001, consensus is to not support empty path
