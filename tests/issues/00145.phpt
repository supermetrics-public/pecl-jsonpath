--TEST--
Test index filter query on input that contains a non-array element
--DESCRIPTION--
https://github.com/supermetrics-public/pecl-jsonpath/issues/145
--SKIPIF--
<?php if (!extension_loaded("jsonpath")) print "skip"; ?>
--FILE--
<?php

$jsonPath = new \JsonPath\JsonPath();
$jsonPath->find(['test_string'], '$.*[61]');
?>
--EXPECT--