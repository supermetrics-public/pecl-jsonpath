--TEST--
Test filter expression with empty expression
--SKIPIF--
<?php if (!extension_loaded("jsonpath")) print "skip"; ?>
--FILE--
<?php

$data = [
    1,
    [
        "key" => 42,
    ],
    "value",
    null,
];

$jsonPath = new JsonPath();
$result = $jsonPath->find($data, "$[?()]");

echo "Assertion 1\n";
var_dump($result);
?>
--EXPECT--
PHP Fatal Error
--XFAIL--
Now returns false, would be better to error out due to invalid syntax
