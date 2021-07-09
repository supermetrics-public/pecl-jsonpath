--TEST--
Test typical use cases
--SKIPIF--
<?php if (!extension_loaded("jsonpath")) print "skip"; ?>
--FILE--
<?php

$jsonPath = new \JsonPath\JsonPath();

$obj = array(
	'id' => 1234,
	'name' => 'just a name',
	'source' => null,
	'source2' => array(
		'child1' => null
	)
);

echo "Assertion 1\n";
var_dump($jsonPath->find($obj, '$.source.id'));
echo "Assertion 2\n";
var_dump($jsonPath->find($obj, '$.source2.child1.id'));
--EXPECT--
Assertion 1
bool(false)
Assertion 2
bool(false)
