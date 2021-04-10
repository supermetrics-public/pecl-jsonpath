--TEST--
Test dot notation without dot
--SKIPIF--
<?php if (!extension_loaded("jsonpath")) print "skip"; ?>
--FILE--
<?php

$data = [
    "a" => 1,
    "\$a" => 2,
];

$jsonPath = new JsonPath();
$result = $jsonPath->find($data, "\$a");

echo "Assertion 1\n";
var_dump($result);
?>
--EXPECT--
PHP Fatal Error
--XFAIL--
Now returns false, would be better to error out due to invalid syntax
