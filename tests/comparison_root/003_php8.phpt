--TEST--
Test root on scalar false
--SKIPIF--
<?php if (!extension_loaded("jsonpath")) print "skip"; ?>
<?php if (version_compare(PHP_VERSION, '8.0.0', '<')) print "skip"; ?>
--FILE--
<?php

$data = false;

$jsonPath = new JsonPath();
$result = $jsonPath->find($data, "$");

var_dump($result);
?>
--EXPECTF--
Fatal error: Uncaught TypeError: JsonPath::find(): Argument #1 ($data) must be of type array, bool given in %s003_php8.php:%d
Stack trace:
#0 %s003_php8.php(%d): JsonPath->find(false, '$')
#1 {main}
  thrown in %s003_php8.php on line %d
