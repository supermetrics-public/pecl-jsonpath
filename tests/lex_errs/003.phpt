--TEST--
Testing >, <, >= <= operators
--SKIPIF--
<?php if (!extension_loaded("jsonpath")) print "skip"; ?>
--FILE--
<?php

$jsonPath = new JsonPath();

try {
    $jsonPath->find([], '$.book[?(@.id.isbn ! "684832674" || @.author == "Herman Melville")]');
} catch(RuntimeException $e) {
    echo get_class($e) . ": " . $e->getMessage();
}
--EXPECT--
RuntimeException: ! operator missing = at position 20
