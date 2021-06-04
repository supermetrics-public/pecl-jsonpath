--TEST--
Test union with more than FILTER_ARR_LEN elements
--SKIPIF--
<?php if (!extension_loaded("jsonpath")) print "skip"; ?>
--FILE--
<?php
$data = [
    "key" => "value",
    "another" => "entry",
];

$jsonPath = new JsonPath();
$result = $jsonPath->find($data, "$['one','two','three','four','five','six','seven','eight','nine']");

var_dump($result);
?>
--EXPECTF--
Fatal error: Uncaught RuntimeException: Union filter may contain no more than 8 elements in %s
Stack trace:
%s
%s
%s