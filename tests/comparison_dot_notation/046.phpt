--TEST--
Test dot notation with wildcard after recursive descent on null value array
--SKIPIF--
<?php if (!extension_loaded("jsonpath")) print "skip"; ?>
--FILE--
<?php
require_once __DIR__ . '/../utils/sort_recursively.php';

$data = [
    40,
    null,
    42,
];

$jsonPath = new JsonPath();
$result = $jsonPath->find($data, "$..*");
sortRecursively($result);

var_dump($result);
?>
--EXPECT--
array(3) {
  [0]=>
  NULL
  [1]=>
  int(40)
  [2]=>
  int(42)
}
