--TEST--
Test bracket notation with number on string
--SKIPIF--
<?php if (!extension_loaded("jsonpath")) print "skip"; ?>
--FILE--
<?php

$data = "Hello World";

$jsonPath = new \JsonPath\JsonPath();
$result = $jsonPath->find($data, '$[0]');

var_dump($result);
?>
--EXPECTF--
Fatal error: Uncaught TypeError: JsonPath\JsonPath::find(): Argument #1 ($data) must be of type array, string given in %s014.php:%d
Stack trace:
%s
%s
%s
