--TEST--
Test filter expression with dot notation with number
--SKIPIF--
<?php if (!extension_loaded("jsonpath")) print "skip"; ?>
--FILE--
<?php
require_once __DIR__ . '/../utils/sort_recursively.php';

$data = [
    [
        "a" => "first",
        "2" => "second",
        "b" => "third",
    ],
];

$jsonPath = new JsonPath();
$result = $jsonPath->find($data, "$[?(@.2 == 'second')]");
sortRecursively($result);

echo "Assertion 1\n";
var_dump($result);
?>
--EXPECT--
Assertion 1
array(1) {
  [0]=>
  array(3) {
    ["a"]=>
    string(5) "first"
    [2]=>
    string(6) "second"
    ["b"]=>
    string(5) "third"
  }
}
