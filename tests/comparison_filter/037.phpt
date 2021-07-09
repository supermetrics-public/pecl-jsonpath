--TEST--
Test filter expression with equals number for dot notation with star
--SKIPIF--
<?php if (!extension_loaded("jsonpath")) print "skip"; ?>
--DESCRIPTION--
This test case is allowed by the grammar but is semantically ambiguous. When
an expression operand resolves to a non-scalar value, the first element in the
set is used for comparison. In this case, the following comparisons are made:

1 == 2
2 == 2
1 == 2
2 == 2
1 == 2
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

$jsonPath = new \JsonPath\JsonPath();
$result = $jsonPath->find($data, "$[?(@.*==2)]");

var_dump($result);
?>
--EXPECT--
array(2) {
  [0]=>
  array(2) {
    [0]=>
    int(2)
    [1]=>
    int(3)
  }
  [1]=>
  array(1) {
    [0]=>
    int(2)
  }
}