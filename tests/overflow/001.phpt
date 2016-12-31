--TEST--
Ensure order of operations are enforced
--SKIPIF--
<?php if (!extension_loaded("jsonpath")) print "skip"; ?>
--FILE--
<?php

$jsonPath = new JsonPath();

try {
    var_dump($jsonPath->find([], '$.test[?(@.aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa == "true")]'));
} catch(RuntimeException $e) {
    echo get_class($e) . ": " . $e->getMessage();
}
--EXPECT--
RuntimeException: String size exceeded 100 bytes at position 143
