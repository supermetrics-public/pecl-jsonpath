--TEST--
Ensure exception is thrown for missing closing bracket
--SKIPIF--
<?php if (!extension_loaded("jsonpath")) print "skip"; ?>
--FILE--
<?php

$jsonPath = new JsonPath();

$jsonPath->find([], '$.book[');
--EXPECTF--
Fatal error: Uncaught RuntimeException: Query contains unbalanced parens/brackets in %s
Stack trace:
%s
%s
%s