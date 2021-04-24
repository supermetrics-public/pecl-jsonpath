--TEST--
Ensure exception is thrown for missing equals sign for != operator
--SKIPIF--
<?php if (!extension_loaded("jsonpath")) print "skip"; ?>
--FILE--
<?php

$jsonPath = new JsonPath();

$jsonPath->find([], '$.book[?(@.id.isbn ! "684832674" || @.author == "Herman Melville")]');
--EXPECTF--
Fatal error: Uncaught RuntimeException: ! operator missing = at position 20 in %s
Stack trace:
%s
%s
%s