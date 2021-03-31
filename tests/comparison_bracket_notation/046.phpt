--TEST--
Test bracket notation without quotes
--SKIPIF--
<?php if (!extension_loaded("jsonpath")) print "skip"; ?>
--FILE--
<?php

$data = [
    "key" => "value",
];

$jsonPath = new JsonPath();
$result = $jsonPath->find($data, "$[key]");

echo "Assertion 1\n";
var_dump($result);
?>
--EXPECTF--
Fatal error: Uncaught RuntimeException: Filter must not be empty in %s046.php:%d
Stack trace:
#0 %s046.php(%d): JsonPath->find(Array, '$[key]')
#1 {main}
  thrown in %s046.php on line %d
