--TEST--
Test filter expression with set wise comparison to scalar
--SKIPIF--
<?php if (!extension_loaded("jsonpath")) print "skip"; ?>
--DESCRIPTION--
This test case is allowed by the grammar but is semantically ambiguous. When
an expression operand resolves to a non-scalar value, the first element in the
set is used for comparison. In this case, the following comparisons are made:

1 >= 4
3 >= 4
5 >= 4
--FILE--
<?php

$data = [
    [
        1,
        2,
    ],
    [
        3,
        4,
    ],
    [
        5,
        6,
    ],
];

$jsonPath = new \JsonPath\JsonPath();
$result = $jsonPath->find($data, "$[?(@[*]>=4)]");

var_dump($result);
?>
--EXPECT--
array(1) {
  [0]=>
  array(2) {
    [0]=>
    int(5)
    [1]=>
    int(6)
  }
}