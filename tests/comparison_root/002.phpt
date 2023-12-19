--TEST--
Test root on scalar
--SKIPIF--
<?php if (!extension_loaded("jsonpath")) print "skip"; ?>
<?php if (version_compare(PHP_VERSION, '8.0.0', '<')) print "skip"; ?>
--FILE--
<?php

$data = 42;

$jsonPath = new \JsonPath\JsonPath();
$result = $jsonPath->find($data, "$");

var_dump($result);
?>
--EXPECTF--
Fatal error: Uncaught TypeError: JsonPath\JsonPath::find(): Argument #1 ($data) must be of type array, int given in %s002.php:%d
Stack trace:
%s
%s
%s
