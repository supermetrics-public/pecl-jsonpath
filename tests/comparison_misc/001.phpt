--TEST--
Test empty
--SKIPIF--
<?php if (!extension_loaded("jsonpath")) print "skip"; ?>
--FILE--
<?php

$data = [
    "a" => 42,
    "" => 21,
];

$jsonPath = new JsonPath();
$result = $jsonPath->find($data, "");

echo "Assertion 1\n";
var_dump($result);
?>
--EXPECT--
PHP Fatal Error
--XFAIL--
Now returns false, would be better to error out due to invalid syntax
