--TEST--
Ensure AST selector on right side of AND expression is not identified as ISSET
--SKIPIF--
<?php if (!extension_loaded("jsonpath")) print "skip"; ?>
--FILE--
<?php

$obj = array(
    'book' =>
    array(
        array(
            'category' => 'fiction',
            'author' => 'J. R. R. Tolkien',
            'title' => 'The Lord of the Rings',
            'price' => 22.99,
            'id' =>
            array(
                'isbn' => '0-395-19395-8',
            )
        ),
        array(
            'category' => 'reference',
            'author' => 'Nigel Rees',
            'title' => 'Sayings of the Century',
            'price' => 8.9499999999999993,
            'id' =>
            array(
                'isbn' => '684832674',
            )
        )
    )
);

$jsonPath = new \JsonPath\JsonPath();

print_r($jsonPath->find($obj, '$.book[?(@["id"]["isbn"] && "Nigel Rees" == @.author)]'));

?>
--EXPECT--
Array
(
    [0] => Array
        (
            [category] => reference
            [author] => Nigel Rees
            [title] => Sayings of the Century
            [price] => 8.95
            [id] => Array
                (
                    [isbn] => 684832674
                )

        )

)