--TEST--
Ensure exception is thrown for missing closing bracket
--SKIPIF--
<?php if (!extension_loaded("jsonpath")) print "skip"; ?>
--FILE--
<?php

$jsonPath = new \JsonPath\JsonPath();

$jsonPath->find([], '$.store.book[?(@.author == "Evelyn Waugh"');
--EXPECTF--
Fatal error: Uncaught JsonPath\JsonPathException: Missing closing paren `)` in %s
Stack trace:
%s
%s
%s