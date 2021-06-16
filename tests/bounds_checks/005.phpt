--TEST--
Test index filter with more than FILTER_ARR_LEN elements
--SKIPIF--
<?php if (!extension_loaded("jsonpath")) print "skip"; ?>
--FILE--
<?php
$data = [
    "key" => "value",
    "another" => "entry",
];

$jsonPath = new JsonPath();
$result = $jsonPath->find($data, "$[0,1,2,3,4,5,6,7,8]");

var_dump($result);
?>
--EXPECTF--
Fatal error: Uncaught RuntimeException: Index filter may contain no more than 8 elements in %s
Stack trace:
%s
%s
%s