--TEST--
Ensure exception is thrown for missing closing bracket
--SKIPIF--
<?php if (!extension_loaded("jsonpath")) print "skip"; ?>
--FILE--
<?php

$jsonPath = new JsonPath();

$jsonPath->find([], '$.testl["test"');
--EXPECTF--
Fatal error: Uncaught RuntimeException: Missing filter end `]` in %s
Stack trace:
%s
%s
%s