--TEST--
Test isset as single operator in expression
--SKIPIF--
<?php if (!extension_loaded("jsonpath")) print "skip"; ?>
--FILE--
<?php

$obj = array(
    'book' =>
    array(
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

print_r($jsonPath->find($obj, '$.book[?(@["id"]["isbn"])]'));

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