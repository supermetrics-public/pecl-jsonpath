--TEST--
Test script expression
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
$result = $jsonPath->find($data, "$[(@.length-1)]");

echo "Assertion 1\n";
var_dump($result);
?>
--EXPECTF--
Fatal error: Uncaught RuntimeException: Missing filter end ] in %s003.php:%d
Stack trace:
#0 %s003.php(%d): JsonPath->find(Array, '$[(@.length-1)]')
#1 {main}
  thrown in %s003.php on line %d
