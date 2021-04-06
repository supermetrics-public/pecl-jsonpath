--TEST--
Test dot notation with single quotes
--SKIPIF--
<?php if (!extension_loaded("jsonpath")) print "skip"; ?>
--FILE--
<?php

$data = [
    "key" => "value",
    "'key'" => 42,
];

$jsonPath = new JsonPath();
$result = $jsonPath->find($data, "$.'key'");

echo "Assertion 1\n";
var_dump($result);
?>
--EXPECT--
PHP Fatal Error
--XFAIL--
Now returns false, would be better to error out due to invalid syntax
