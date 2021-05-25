--TEST--
Test regex without a match
--SKIPIF--
<?php if (!extension_loaded("jsonpath")) print "skip"; ?>
--FILE--
<?php

$jsonPath = new JsonPath();

$obj = array (
  'test' => array(
      array (
        'id' => 1,
        'val' => 10,
        'val_str' => "PHP is the web scripting language of choice"
      ),
   )
);

var_dump($jsonPath->find($obj, '$.test[?(@.val_str =~ "/notfound/")]'));
--EXPECT--
bool(false)
