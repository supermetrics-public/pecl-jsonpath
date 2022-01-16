--TEST--
Test child filter on recursive wildcard
--DESCRIPTION--
https://github.com/supermetrics-public/pecl-jsonpath/issues/158
--SKIPIF--
<?php if (!extension_loaded("jsonpath")) print "skip"; ?>
--FILE--
<?php

$obj = array (
         'store' =>
         array (
           'book' =>
           array (
             0 =>
             array (
               'category' => 'reference',
               'title' => 'Sayings of the Century',
               'price' => 8.9499999999999993,
               'author' => 'Nigel Rees',
             ),
             1 =>
             array (
               'category' => 'fiction',
               'author' => 'Evelyn Waugh',
               'title' => 'Sword of Honour',
               'price' => 12.99,
             ),
             2 =>
             array (
               'category' => 'fiction',
               'author' => 'Herman Melville',
               'title' => 'Moby Dick',
               'isbn' => '0-553-21311-3',
               'price' => 8.9900000000000002,
             ),
             3 =>
             array (
               'category' => 'fiction',
               'author' => 'J. R. R. Tolkien',
               'title' => 'The Lord of the Rings',
               'isbn' => '0-395-19395-8',
               'price' => 22.989999999999998,
             ),
           ),
           'bicycle' =>
           array (
             'color' => 'red',
             'price' => 19.949999999999999,
           ),
           'price' =>
           array (
             0 => 1,
             1 => 2,
             2 => 3,
             3 =>
             array (
               0 => 1,
             ),
             4 =>
             array (
               'price' => 'EMBEDDED',
             ),
           ),
         ),
         'expensive' => 10,
         'title' => 'WHAT',
         'hello' =>
         array (
           'store' =>
           array (
             0 =>
             array (
               'title' => 'HAH!',
             ),
           ),
         ),
         'book' =>
         array (
           0 =>
           array (
             'category' => 'reference',
             'author' => 'Nigel Rees',
             'title' => 'Sayings of the Century',
             'price' => 8.9499999999999993,
             'id' =>
             array (
               'isbn' => '684832674',
             ),
           ),
         ),
       );

$jsonPath = new \JsonPath\JsonPath();

print_r($jsonPath->find($obj, '$..*["category"]'));
--EXPECT--
Array
(
    [0] => reference
    [1] => fiction
    [2] => fiction
    [3] => fiction
    [4] => reference
)