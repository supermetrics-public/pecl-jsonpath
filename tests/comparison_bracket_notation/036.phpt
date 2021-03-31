--TEST--
Test bracket notation with two literals separated by dot
--SKIPIF--
<?php if (!extension_loaded("jsonpath")) print "skip"; ?>
--FILE--
<?php

$data = [
    "one" => [
        "key" => "value",
    ],
    "two" => [
        "some" => "more",
        "key" => "other value",
    ],
    "two.some" => "42",
    "two'.'some" => "43",
];

$jsonPath = new JsonPath();
$result = $jsonPath->find($data, "$['two'.'some']");

echo "Assertion 1\n";
var_dump($result);
?>
--EXPECTF--
Fatal error: Uncaught RuntimeException: Missing closing ] bracket at position 7 in %s036.php:%d
Stack trace:
#0 %s036.php(%d): JsonPath->find(Array, '$['two'.'some']')
#1 {main}
  thrown in %s036.php on line %d
