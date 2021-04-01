--TEST--
Test bracket notation with two literals separated by dot without quotes
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
];

$jsonPath = new JsonPath();
$result = $jsonPath->find($data, "$[two.some]");

echo "Assertion 1\n";
var_dump($result);
?>
--EXPECTF--
Fatal error: Uncaught RuntimeException: Missing filter end ] in %s037.php:%d
Stack trace:
#0 %s037.php(%d): JsonPath->find(Array, '$[two.some]')
#1 {main}
  thrown in %s037.php on line %d
