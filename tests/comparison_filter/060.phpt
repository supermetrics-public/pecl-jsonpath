--TEST--
Test filter expression with set wise comparison to set
--SKIPIF--
<?php if (!extension_loaded("jsonpath")) print "skip"; ?>
--DESCRIPTION--
This test case is allowed by the grammar but is semantically ambiguous. When
an expression operand resolves to a non-scalar value, the first element in the
set is used for comparison. In this case, the following comparisons are made:

1 >= 3
3 >= 3
5 >= 3
--FILE--
<?php

$data = [
    "x" => [
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
    ],
    "y" => [
        3,
        4,
        5,
    ],
];

$jsonPath = new JsonPath();
$result = $jsonPath->find($data, "$.x[?(@[*]>=$.y[*])]");

var_dump($result);
?>
--EXPECT--
array(2) {
  [0]=>
  array(2) {
    [0]=>
    int(3)
    [1]=>
    int(4)
  }
  [1]=>
  array(2) {
    [0]=>
    int(5)
    [1]=>
    int(6)
  }
}