--TEST--
Testing != operator
--SKIPIF--
<?php if (!extension_loaded("jsonpath")) print "skip"; ?>
--FILE--
<?php

$jsonPath = new \JsonPath\JsonPath();

$obj = array (
  'test' => array(
      array (
        'id' => 1,
        'val' => 10,
        'val_str' => "table"
      ),
   )
);

var_dump($jsonPath->find($obj, '$.test[?(@.val != 10)]'));
--EXPECT--
bool(false)
