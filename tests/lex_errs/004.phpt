--TEST--
Ensure exception is thrown for missing double &&
--SKIPIF--
<?php if (!extension_loaded("jsonpath")) print "skip"; ?>
--FILE--
<?php

$jsonPath = new \JsonPath\JsonPath();

$jsonPath->find([], '$.book[?(@.id.isbn == "684832674" & @.author == "Herman Melville")]');
--EXPECTF--
Fatal error: Uncaught RuntimeException: AND operator must be double ampersand `&&` at position 35 in %s
Stack trace:
%s
%s
%s