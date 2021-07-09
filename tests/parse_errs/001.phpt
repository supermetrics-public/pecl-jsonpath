--TEST--
Ensure exception is thrown for missing closing bracket
--SKIPIF--
<?php if (!extension_loaded("jsonpath")) print "skip"; ?>
--FILE--
<?php

$jsonPath = new \JsonPath\JsonPath();

$jsonPath->find([], '$.book[');
--EXPECTF--
Fatal error: Uncaught RuntimeException: Missing filter end `]` in %s
Stack trace:
%s
%s
%s