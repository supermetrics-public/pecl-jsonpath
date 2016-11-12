--TEST--
Ensure no segfault if filtering on object instead of array
--SKIPIF--
<?php if (!extension_loaded("jsonpath")) print "skip"; ?>
--FILE--
<?php

$jsonPath = new JsonPath();

$obj = array(
	'test' => array(
		'val' => 0
	)
);


var_dump($jsonPath->find($obj, '$.test[?(@.vals == 0)]'));
--EXPECT--
bool(false)
