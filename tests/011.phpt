--TEST--
Testing >= operator
--SKIPIF--
<?php if (!extension_loaded("jsonpath")) print "skip"; ?>
--FILE--
<?php

$jsonPath = new JsonPath();

$obj = array (
  'test' => array(
      array (
        'id' => 1,
        'val' => 11,
        'val_str' => "table"
      ),
   )
);

var_dump($jsonPath->find($obj, '$.test[?(@.val >= 11)]'));
--EXPECT--
array(1) {
  [0]=>
  array(3) {
    ["id"]=>
    int(1)
    ["val"]=>
    int(11)
    ["val_str"]=>
    string(5) "table"
  }
}
