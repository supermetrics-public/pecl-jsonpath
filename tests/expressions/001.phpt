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
			"a" => "false",
			"b" => "false",
			"c" => "false",
			"d" => "true"
		]
	]
];

echo "Without parentheses\n";
var_dump($jsonPath->find($obj, '$.test[?(@.a == "true" || @.b == "true" && @.c == "true" || @.d == "true")]'));
echo "With parentheses\n";
var_dump($jsonPath->find($obj, '$.test[?(@.a == "true" || @.b == "true" && (@.c == "true" || @.d == "true"))]'));
--EXPECT--
Without parentheses
array(1) {
  [0]=>
  array(4) {
    ["a"]=>
    string(5) "false"
    ["b"]=>
    string(5) "false"
    ["c"]=>
    string(5) "false"
    ["d"]=>
    string(4) "true"
  }
}
With parentheses
bool(false)
