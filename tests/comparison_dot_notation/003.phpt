--TEST--
Test dot bracket notation without quotes
--SKIPIF--
<?php if (!extension_loaded("jsonpath")) print "skip"; ?>
--FILE--
<?php

$data = [
    "key" => "value",
    "other" => [
        "key" => [
            "key" => 42,
        ],
    ],
];

$jsonPath = new JsonPath();
$result = $jsonPath->find($data, "$.[key]");

echo "Assertion 1\n";
var_dump($result);
?>
--EXPECTF--
Fatal error: Uncaught RuntimeException: Filter must not be empty in %s003.php:%d
Stack trace:
#0 %s003.php(%d): JsonPath->find(Array, '$.[key]')
#1 {main}
  thrown in %s003.php on line %d
