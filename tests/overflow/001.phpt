--TEST--
Ensure selector name string length does not exceed buffer
--SKIPIF--
<?php if (!extension_loaded("jsonpath")) print "skip"; ?>
--FILE--
<?php

$jsonPath = new JsonPath();

var_dump($jsonPath->find([], '$.test[?(@.aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa == "true")]'));
--EXPECTF--
Fatal error: Uncaught RuntimeException: String exceeded buffer size at position 61 in %s
Stack trace:
%s
%s
%s