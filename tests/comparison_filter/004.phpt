--TEST--
Test filter expression with addition
--SKIPIF--
<?php if (!extension_loaded("jsonpath")) print "skip"; ?>
--FILE--
<?php

$data = [
    [
        "key" => 60,
    ],
    [
        "key" => 50,
    ],
    [
        "key" => 10,
    ],
    [
        "key" => -50,
    ],
    [
        "key+50" => 100,
    ],
];

$jsonPath = new JsonPath();
$result = $jsonPath->find($data, "$[?(@.key+50==100)]");

echo "Assertion 1\n";
var_dump($result);
?>
--EXPECT--
PHP Fatal Error
--XFAIL--
Now returns false, would be better to error out due to invalid syntax
