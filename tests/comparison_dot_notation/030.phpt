--TEST--
Test dot notation with key root literal
--SKIPIF--
<?php if (!extension_loaded("jsonpath")) print "skip"; ?>
--FILE--
<?php

$data = [
    "$" => "value",
];

$jsonPath = new JsonPath();
$result = $jsonPath->find($data, "$.$");

echo "Assertion 1\n";
var_dump($result);
?>
--EXPECT--
Assertion 1
array(1) {
  [0]=>
  string(5) "value"
}
--XFAIL--
Requires support for a wider range of characters in node names
