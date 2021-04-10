--TEST--
Test dot notation without root
--SKIPIF--
<?php if (!extension_loaded("jsonpath")) print "skip"; ?>
--FILE--
<?php

$data = [
    "key" => "value",
];

$jsonPath = new JsonPath();
$result = $jsonPath->find($data, ".key");

echo "Assertion 1\n";
var_dump($result);
?>
--EXPECT--
PHP Fatal Error
--XFAIL--
Now returns value, would be better to error out due to invalid syntax
