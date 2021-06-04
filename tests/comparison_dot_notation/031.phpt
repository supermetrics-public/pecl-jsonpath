--TEST--
Test dot notation with non ASCII key
--SKIPIF--
<?php if (!extension_loaded("jsonpath")) print "skip"; ?>
--FILE--
<?php

$data = [
    "屬性" => "value",
];

$jsonPath = new JsonPath();
$result = $jsonPath->find($data, "$.屬性");

var_dump($result);
?>
--EXPECT--
array(1) {
  [0]=>
  string(5) "value"
}
