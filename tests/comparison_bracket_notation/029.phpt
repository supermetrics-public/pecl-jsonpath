--TEST--
Test bracket notation with quoted special characters combined
--SKIPIF--
<?php if (!extension_loaded("jsonpath")) print "skip"; ?>
--FILE--
<?php

$data = [
    ":@.\"$,*'\\" => 42,
];

$jsonPath = new JsonPath();
$result = $jsonPath->find($data, "$[':@.\"$,*\'\\\\']");

echo "Assertion 1\n";
var_dump($result);
?>
--EXPECT--
Assertion 1
array(1) {
  [0]=>
  int(42)
}
--XFAIL--
Requires changes to accommodate for quoted single-quotes
