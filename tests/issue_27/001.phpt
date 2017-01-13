--TEST--
Test typical use cases
--SKIPIF--
<?php if (!extension_loaded("jsonpath")) print "skip"; ?>
--FILE--
<?php

$jsonPath = new JsonPath();

$obj = array (
  'store' => 
  array (
    'book' => 
    array (
      0 => 
      array (
        'category' => 'reference',
        'author' => 'Nigel Rees',
        'title' => 'Sayings of the Century',
        'price' => 8.95,
        'available' => true,
      ),
      1 => 
      array (
        'category' => 'fiction',
        'author' => 'Evelyn Waugh',
        'title' => 'Sword of Honour',
        'price' => 12.99,
        'available' => false,
      ),
      2 => 
      array (
        'category' => 'fiction',
        'author' => 'Herman Melville',
        'title' => 'Moby Dick',
        'isbn' => '0-553-21311-3',
        'price' => 8.99,
        'available' => true,
      ),
      3 => 
      array (
        'category' => 'fiction',
        'author' => 'J. R. R. Tolkien',
        'title' => 'The Lord of the Rings',
        'isbn' => '0-395-19395-8',
        'price' => 22.99,
        'available' => false,
      ),
    ),
    'bicycle' => 
    array (
      'color' => 'red',
      'price' => 19.95,
      'available' => true,
    ),
  ),
  'authors' => 
  array (
    0 => 'Nigel Rees',
    1 => 'Evelyn Waugh',
    2 => 'Herman Melville',
    3 => 'J. R. R. Tolkien',
  ),
);

print_r($jsonPath->find($obj, "$..book[?(@.category == 'fiction')]"));
print_r($jsonPath->find($obj, "$..book[?(@.price < 10)].title"));
print_r($jsonPath->find($obj, "$..book[?(@.category == 'fiction')]"));

?>
--EXPECT--
Array
(
    [0] => Array
        (
            [category] => fiction
            [author] => Evelyn Waugh
            [title] => Sword of Honour
            [price] => 12.99
            [available] => 
        )

    [1] => Array
        (
            [category] => fiction
            [author] => Herman Melville
            [title] => Moby Dick
            [isbn] => 0-553-21311-3
            [price] => 8.99
            [available] => 1
        )

    [2] => Array
        (
            [category] => fiction
            [author] => J. R. R. Tolkien
            [title] => The Lord of the Rings
            [isbn] => 0-395-19395-8
            [price] => 22.99
            [available] => 
        )

)
Array
(
    [0] => Sayings of the Century
    [1] => Moby Dick
)
Array
(
    [0] => Array
        (
            [category] => fiction
            [author] => Evelyn Waugh
            [title] => Sword of Honour
            [price] => 12.99
            [available] => 
        )

    [1] => Array
        (
            [category] => fiction
            [author] => Herman Melville
            [title] => Moby Dick
            [isbn] => 0-553-21311-3
            [price] => 8.99
            [available] => 1
        )

    [2] => Array
        (
            [category] => fiction
            [author] => J. R. R. Tolkien
            [title] => The Lord of the Rings
            [isbn] => 0-395-19395-8
            [price] => 22.99
            [available] => 
        )

)
