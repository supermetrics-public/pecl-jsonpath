--TEST--
Test binary operator with missing left-hand operand
--DESCRIPTION--
https://github.com/supermetrics-public/pecl-jsonpath/issues/143
--SKIPIF--
<?php if (!extension_loaded("jsonpath")) print "skip"; ?>
--FILE--
<?php

$jsonPath = new \JsonPath\JsonPath();
$jsonPath->find([], '$.store.book[?(@.author == "Herman Melville" &&<@.t)tle == "Moby Dick" || @.or == "Evelyn Wa")]');
?>
--EXPECTF--
Fatal error: Uncaught JsonPath\JsonPathException: Expecting child node, filter, expression, or recursive node in %s
Stack trace:
%s
%s
%s