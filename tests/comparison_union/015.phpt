--TEST--
Test union with wildcard and number
--SKIPIF--
<?php if (!extension_loaded("jsonpath")) print "skip"; ?>
--FILE--
<?php

$data = [
    "first",
    "second",
    "third",
    "forth",
    "fifth",
];

$jsonPath = new JsonPath();
$result = $jsonPath->find($data, "$[*,1]");

echo "Assertion 1\n";
var_dump($result);
?>
--EXPECTF--
Fatal error: Uncaught RuntimeException: Missing filter end ] in %s015.php:%d
Stack trace:
#0 %s015.php(%d): JsonPath->find(Array, '$[*,1]')
#1 {main}
  thrown in %s015.php on line %d
