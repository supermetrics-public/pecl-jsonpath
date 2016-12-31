--TEST--
Ensure order of operations are enforced
--SKIPIF--
<?php if (!extension_loaded("jsonpath")) print "skip"; ?>
--FILE--
<?php

$jsonPath = new JsonPath();

$obj = [
	'test' => [
		[
			"a" => "true",
			"b" => "false",
			"c" => "false"
		]
	]
];

echo "Without parentheses\n";
var_dump($jsonPath->find($obj, '$.test[?(@.a == "true" || @.b == "true" && @.c == "true")]'));
echo "With parentheses\n";
var_dump($jsonPath->find($obj, '$.test[?((@.a == "true" || @.b == "true") && @.c == "true")]'));
--EXPECT--
Without parentheses
array(1) {
  [0]=>
  array(3) {
    ["a"]=>
    string(4) "true"
    ["b"]=>
    string(5) "false"
    ["c"]=>
    string(5) "false"
  }
}
With parentheses
bool(false)
