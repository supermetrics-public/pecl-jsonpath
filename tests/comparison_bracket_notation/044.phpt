--TEST--
Test bracket notation with wildcard after dot notation after bracket notation with wildcard
--SKIPIF--
<?php if (!extension_loaded("jsonpath")) print "skip"; ?>
--FILE--
<?php

$data = [
    [
        "bar" => [
            42,
        ],
    ],
];

$jsonPath = new JsonPath();
$result = $jsonPath->find($data, "$[*].bar[*]");

echo "Assertion 1\n";
var_dump($result);
?>
--EXPECT--
Assertion 1
array(1) {
  [0]=>
  int(42)
}
--XFAIL--
Requires more work on the wildcard iteration
