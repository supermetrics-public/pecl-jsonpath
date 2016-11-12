--TEST--
Testing >, <, >= <= operators
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
        'val_str' => "table"
      ),
   )
);


echo "Assertion 1\n";
var_dump($jsonPath->find($obj, '$.test[?(@.val == 10)]'));
echo "Assertion 2\n";
var_dump($jsonPath->find($obj, '$.test[?(@.val < 11)]'));
echo "Assertion 3\n";
var_dump($jsonPath->find($obj, '$.test[?(@.val < 10)]'));
echo "Assertion 4\n";
var_dump($jsonPath->find($obj, '$.test[?(@.val > 10)]'));
echo "Assertion 5\n";
var_dump($jsonPath->find($obj, '$.test[?(@.val_str > "apple")]'));
echo "Assertion 6\n";
var_dump($jsonPath->find($obj, '$.test[?(@.val_str < "zebra")]'));
echo "Assertion 7\n";
var_dump($jsonPath->find($obj, '$.test[?(@.val_str < "table")]'));
echo "Assertion 8\n";
var_dump($jsonPath->find($obj, '$.test[?(@.val_str > "table")]'));
--EXPECT--
Assertion 1
array(1) {
  [0]=>
  array(3) {
    ["id"]=>
    int(1)
    ["val"]=>
    int(10)
    ["val_str"]=>
    string(5) "table"
  }
}
Assertion 2
array(1) {
  [0]=>
  array(3) {
    ["id"]=>
    int(1)
    ["val"]=>
    int(10)
    ["val_str"]=>
    string(5) "table"
  }
}
Assertion 3
bool(false)
Assertion 4
bool(false)
Assertion 5
array(1) {
  [0]=>
  array(3) {
    ["id"]=>
    int(1)
    ["val"]=>
    int(10)
    ["val_str"]=>
    string(5) "table"
  }
}
Assertion 6
array(1) {
  [0]=>
  array(3) {
    ["id"]=>
    int(1)
    ["val"]=>
    int(10)
    ["val_str"]=>
    string(5) "table"
  }
}
Assertion 7
bool(false)
Assertion 8
bool(false)
