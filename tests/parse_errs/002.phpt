--TEST--
Testing >, <, >= <= operators
--SKIPIF--
<?php if (!extension_loaded("jsonpath")) print "skip"; ?>
--FILE--
<?php

$jsonPath = new JsonPath();

try {
    $jsonPath->find([], '$.store.book[?(@.author == "Evelyn Waugh"');
} catch(RuntimeException $e) {
    echo get_class($e) . ": " . $e->getMessage();
}
--EXPECT--
RuntimeException: Missing filter end ]
