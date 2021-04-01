--TEST--
Test bracket notation with number on string
--SKIPIF--
<?php if (!extension_loaded("jsonpath")) print "skip"; ?>
<?php if (version_compare(PHP_VERSION, '8.0.0', '>=')) print "skip"; ?>
--FILE--
<?php

$data = "Hello World";

$jsonPath = new JsonPath();
$result = $jsonPath->find($data, '$[0]');

echo "Assertion 1\n";
var_dump($result);
?>
--EXPECTF--
Warning: JsonPath::find() expects parameter 1 to be array, string given in %s014_php7.php on line %d
Assertion 1
NULL
