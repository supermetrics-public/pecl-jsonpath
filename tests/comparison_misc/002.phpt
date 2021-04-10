--TEST--
Test parens notation
--SKIPIF--
<?php if (!extension_loaded("jsonpath")) print "skip"; ?>
--FILE--
<?php

$data = [
    "key" => 1,
    "some" => 2,
    "more" => 3,
];

$jsonPath = new JsonPath();
$result = $jsonPath->find($data, "$(key,more)");

echo "Assertion 1\n";
var_dump($result);
?>
--EXPECT--
PHP Fatal Error
--XFAIL--
Now returns false, would be better to error out due to invalid syntax
