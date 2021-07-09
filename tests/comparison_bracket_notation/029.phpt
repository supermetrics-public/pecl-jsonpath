--TEST--
Test bracket notation with quoted special characters combined
--SKIPIF--
<?php if (!extension_loaded("jsonpath")) print "skip"; ?>
--FILE--
<?php

$data = [
    ":@.\"$,*'\\" => 42,
];

$jsonPath = new \JsonPath\JsonPath();
$result = $jsonPath->find($data, "$[':@.\"$,*\'\\\\']");

var_dump($result);
?>
--EXPECT--
array(1) {
  [0]=>
  int(42)
}
