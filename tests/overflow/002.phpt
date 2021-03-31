--TEST--
Ensure queries that contain more than PARSE_BUF_LEN tokens safely fail
--SKIPIF--
<?php if (!extension_loaded("jsonpath")) print "skip"; ?>
--FILE--
<?php

$jsonPath = new JsonPath();

try {
    // This query contains 51 tokens (root selector $ plus 50 dot selectors), which exceeds PARSE_BUF_LEN
    var_dump($jsonPath->find([], '$.1.2.3.4.5.6.7.8.9.10.11.12.13.14.15.16.17.18.19.20.21.22.23.24.25.26.27.28.29.30.31.32.33.34.35.36.37.38.39.40.41.42.43.44.45.46.47.48.49.50'));
} catch(RuntimeException $e) {
    echo get_class($e) . ": " . $e->getMessage();
}
--EXPECT--
RuntimeException: The query is too long. Token count exceeds PARSE_BUF_LEN.