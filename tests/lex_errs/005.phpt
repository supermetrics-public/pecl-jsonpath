--TEST--
Ensure exception is thrown for missing double ||
--SKIPIF--
<?php if (!extension_loaded("jsonpath")) print "skip"; ?>
--FILE--
<?php

$jsonPath = new JsonPath();

try {
    $jsonPath->find([], '$.book[?(@.id.isbn == "684832674" | @.author == "Herman Melville")]');
} catch(RuntimeException $e) {
    echo get_class($e) . ": " . $e->getMessage();
}
--EXPECT--
RuntimeException: 'Or' operator must be double || at position 35
