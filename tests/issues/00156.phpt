--TEST--
Test empty filter operand with negation operator within expression
--DESCRIPTION--
https://github.com/supermetrics-public/pecl-jsonpath/issues/156
--SKIPIF--
<?php if (!extension_loaded("jsonpath")) print "skip"; ?>
--FILE--
<?php

$jsonPath = new \JsonPath\JsonPath();
$jsonPath->find(["test" => [["id" => 1,"val" => 10,"val_str" => "table"]]], "$.test[?(![])]");
?>
--EXPECTF--
Fatal error: Uncaught JsonPath\JsonPathException: Filter must not be empty in %s
Stack trace:
%s
%s
%s