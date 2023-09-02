--TEST--
Test root on scalar true
--SKIPIF--
<?php if (!extension_loaded("jsonpath")) print "skip"; ?>
<?php if (version_compare(PHP_VERSION, '8.0.0', '<')) print "skip"; ?>
--FILE--
<?php

$data = true;

$jsonPath = new \JsonPath\JsonPath();
$result = $jsonPath->find($data, "$");

var_dump($result);
?>
--EXPECTF--
Fatal error: Uncaught TypeError: JsonPath\JsonPath::find(): Argument #1 ($data) must be of type array, %s given in %s004_php8.php:%d
Stack trace:
%s
%s
%s
