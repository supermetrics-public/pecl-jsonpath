--TEST--
Ensure exception is thrown for missing closing bracket
--SKIPIF--
<?php if (!extension_loaded("jsonpath")) print "skip"; ?>
--FILE--
<?php

$jsonPath = new JsonPath();

$jsonPath->find([], '$.store.book[?(@.author == "Evelyn Waugh"');
--EXPECTF--
Fatal error: Uncaught RuntimeException: Missing closing paren ) in %s
Stack trace:
%s
%s
%s