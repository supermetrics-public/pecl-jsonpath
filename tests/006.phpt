--TEST--
Ensure no "Array to string conversion" notice when selecting non-scalar
--SKIPIF--
<?php if (!extension_loaded("jsonpath")) print "skip"; ?>
--FILE--
<?php

$jsonPath = new \JsonPath\JsonPath();

$obj = array(
	'test' => array(
		array(
			'val' => array(1)
		)
	)
);


var_dump($jsonPath->find($obj, '$.test[?(@.val == 0)]'));
--EXPECT--
bool(false)
