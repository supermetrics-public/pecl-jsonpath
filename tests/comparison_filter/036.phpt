--TEST--
Test filter expression with equals number for bracket notation with star
--SKIPIF--
<?php if (!extension_loaded("jsonpath")) print "skip"; ?>
--FILE--
<?php

$data = [
    [
        1,
        2,
    ],
    [
        2,
        3,
    ],
    [
        1,
    ],
    [
        2,
    ],
    [
        1,
        2,
        3,
    ],
    1,
    2,
    3,
];

$jsonPath = new JsonPath();
$result = $jsonPath->find($data, "$[?(@[*]==2)]");

echo "Assertion 1\n";
var_dump($result);
?>
--EXPECT--
??
--XFAIL--
Not sure what would be the best outcome here
