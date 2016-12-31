--TEST--
Ensure exception is thrown for missing closing bracket
--SKIPIF--
<?php if (!extension_loaded("jsonpath")) print "skip"; ?>
--FILE--
<?php

$jsonPath = new JsonPath();

try {
    $jsonPath->find([], '$.testl["test"');
} catch(RuntimeException $e) {
    echo get_class($e) . ": " . $e->getMessage();
}
--EXPECT--
RuntimeException: Missing closing ] bracket at position 14
