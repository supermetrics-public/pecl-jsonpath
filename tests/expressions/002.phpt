--TEST--
Test JSONPath query within JSONPath expression
--SKIPIF--
<?php if (!extension_loaded("jsonpath")) print "skip"; ?>
--FILE--
<?php

$json = array(
    'authors' =>
    array(
        'Nigel Rees',
        'J. R. R. Tolkien',
        'Herman Melville',
        'Evelyn Waugh',
    ),
    'store' =>
    array(
        'book' =>
        array(
            array(
                'category' => 'reference',
                'author' => 'Nigel Rees',
                'title' => 'Sayings of the Century',
                'price' => 8.95,
            ),
            array(
                'category' => 'poetry',
                'author' => 'Herman Melville',
                'title' => 'Timoleon',
                'isbn' => '978-0521034135',
                'price' => 42.99,
            ),
            array(
                'category' => 'fiction',
                'author' => 'Evelyn Waugh',
                'title' => 'Sword of Honour',
                'price' => 12.99,
            ),
            array(
                'category' => 'fiction',
                'author' => 'Herman Melville',
                'title' => 'Moby Dick',
                'isbn' => '0-553-21311-3',
                'price' => 8.99,
            ),
            array(
                'category' => 'fiction',
                'author' => 'J. R. R. Tolkien',
                'title' => 'The Lord of the Rings',
                'isbn' => '0-395-19395-8',
                'price' => 22.99,
            )
        )
    )
);

$jsonPath = new \JsonPath\JsonPath();

print_r($jsonPath->find($json, '$..book[?(@.author==$.authors[2])].isbn'));

?>
--EXPECT--
Array
(
    [0] => 978-0521034135
    [1] => 0-553-21311-3
)