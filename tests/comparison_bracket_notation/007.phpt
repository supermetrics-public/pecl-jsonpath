--TEST--
Test bracket notation with empty path
--SKIPIF--
<?php if (!extension_loaded("jsonpath")) print "skip"; ?>
--FILE--
<?php

$data = [
    "" => 42,
    "''" => 123,
    "\"\"" => 222,
];

$jsonPath = new JsonPath();
$result = $jsonPath->find($data, '$[]');

var_dump($result);
?>
--EXPECTF--
Fatal error: Uncaught RuntimeException: Filter must not be empty in %s007.php:%d
Stack trace:
#0 %s007.php(%d): JsonPath->find(Array, '$[]')
#1 {main}
  thrown in %s007.php on line %d
