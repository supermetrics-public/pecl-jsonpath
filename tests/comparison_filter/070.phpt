--TEST--
Test filter expression with value false
--SKIPIF--
<?php if (!extension_loaded("jsonpath")) print "skip"; ?>
--FILE--
<?php

$data = [
    1,
    3,
    "nice",
    true,
    null,
    false,
    [],
    [],
    -1,
    0,
    "",
];

$jsonPath = new JsonPath();
$result = $jsonPath->find($data, "$[?(false)]");

echo "Assertion 1\n";
var_dump($result);
?>
--EXPECT--
PHP Fatal Error
--XFAIL--
Now returns false, would be better to error out due to invalid syntax
