--TEST--
Test typical use cases
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

echo "Assertion 1\n";
print_r($jsonPath->find($obj, '$.store.book'));

echo "Assertion 2\n";
print_r($jsonPath->find($obj, "$['store']['book']"));

echo "Assertion 3\n";
print_r($jsonPath->find($obj, '$.title'));

echo "Assertion 4\n";
print_r($jsonPath->find($obj, "$['title']"));

echo "Assertion 5\n";
print_r($jsonPath->find($obj, '$..store..title'));

echo "Assertion 6\n";
print_r($jsonPath->find($obj, "$..['store']..['title']"));

echo "Assertion 7\n";
print_r($jsonPath->find($obj, '$..title'));

echo "Assertion 8\n";
print_r($jsonPath->find($obj, "$..['title']"));

echo "Assertion 9\n";
print_r($jsonPath->find($obj, '$.book[0,3]'));

echo "Assertion 10\n";
print_r($jsonPath->find($obj, "$['book'][0,3]"));

echo "Assertion 11\n";
print_r($jsonPath->find($obj, '$.book[0,1,2].category'));

echo "Assertion 12\n";
print_r($jsonPath->find($obj, '$.book[0:2]'));

echo "Assertion 13\n";
print_r($jsonPath->find($obj, '$.book[0:2].category'));

echo "Assertion 14\n";
print_r($jsonPath->find($obj, '$.book[*]'));

echo "Assertion 15\n";
print_r($jsonPath->find($obj, '$.book[*].category'));

echo "Assertion 16\n";
print_r($jsonPath->find($obj, '$.store.*'));

echo "Assertion 17\n";
print_r($jsonPath->find($obj, '$..book[1:3].title'));

echo "Assertion 18\n";
print_r($jsonPath->find($obj, '$..book[1,3].title'));

echo "Assertion 19\n";
print_r($jsonPath->find($obj, '$..book[*].title'));

echo "Assertion 20\n";
print_r($jsonPath->find($obj, '$.store.book[?(@.author == "Evelyn Waugh")]'));

echo "Assertion 21\n";
print_r($jsonPath->find($obj, '$.store.book[?(@.author == "Evelyn Waugh" || @.author == "Nigel Rees")]'));

echo "Assertion 22\n";
print_r($jsonPath->find($obj, '$.store.book[?(@.author == "Nigel Rees" || @.title == "Moby Dick")]'));

echo "Assertion 23\n";
print_r($jsonPath->find($obj, '$.store.book[?(@.author == "Herman Melville" && @.title == "Moby Dick")]'));

echo "Assertion 24\n";
print_r($jsonPath->find($obj, '$.store.book[?(@.category == "fiction")]')); //Returns 3 of a kind

echo "Assertion 25\n";
print_r($jsonPath->find($obj, 
    '$.store.book[?(@.author == "Herman Melville" && @.title == "Moby Dick" || @.author == "Evelyn Waugh")]'
    ));

echo "Assertion 26\n";
print_r($jsonPath->find($obj, '$.book[?(@.id.isbn == "684832674")]'));

echo "Assertion 27\n";
print_r($jsonPath->find($obj, '$.book[?(@.id.isbn == "684832674" && @.author == "Nigel Rees")]'));

echo "Assertion 28\n";
print_r($jsonPath->find($obj, '$.book[?(@.id.isbn == "684832674" || @.author == "Herman Melville")]'));

echo "Assertion 29\n";
print_r($jsonPath->find($obj, '$.store.book[?(@["author"] == "Evelyn Waugh")]'));

echo "Assertion 30\n";
print_r($jsonPath->find($obj, "$.store.book[?(@['author'] == 'Evelyn Waugh')]"));

echo "Assertion 31\n";
print_r($jsonPath->find($obj, '$.book[?(@["id"]["isbn"] == "684832674")]'));

echo "Assertion 32\n";
print_r($jsonPath->find($obj, "$.book[?(@['id']['isbn'] == '684832674')]"));

echo "Assertion 33\n";
print_r($jsonPath->find($obj, '$.book[?(@["id"]["isbn"] == "684832674" || @.author == "Herman Melville")]'));

echo "Assertion 34\n";
print_r($jsonPath->find($obj, '$.book[?(@["id"]["isbn"] && @.author == "Nigel Rees")]'));

?>
--EXPECT--
Assertion 1
Array
(
    [0] => Array
        (
            [0] => Array
                (
                    [category] => reference
                    [title] => Sayings of the Century
                    [price] => 8.95
                    [author] => Nigel Rees
                )

            [1] => Array
                (
                    [category] => fiction
                    [author] => Evelyn Waugh
                    [title] => Sword of Honour
                    [price] => 12.99
                )

            [2] => Array
                (
                    [category] => fiction
                    [author] => Herman Melville
                    [title] => Moby Dick
                    [isbn] => 0-553-21311-3
                    [price] => 8.99
                )

            [3] => Array
                (
                    [category] => fiction
                    [author] => J. R. R. Tolkien
                    [title] => The Lord of the Rings
                    [isbn] => 0-395-19395-8
                    [price] => 22.99
                )

        )

)
Assertion 2
Array
(
    [0] => Array
        (
            [0] => Array
                (
                    [category] => reference
                    [title] => Sayings of the Century
                    [price] => 8.95
                    [author] => Nigel Rees
                )

            [1] => Array
                (
                    [category] => fiction
                    [author] => Evelyn Waugh
                    [title] => Sword of Honour
                    [price] => 12.99
                )

            [2] => Array
                (
                    [category] => fiction
                    [author] => Herman Melville
                    [title] => Moby Dick
                    [isbn] => 0-553-21311-3
                    [price] => 8.99
                )

            [3] => Array
                (
                    [category] => fiction
                    [author] => J. R. R. Tolkien
                    [title] => The Lord of the Rings
                    [isbn] => 0-395-19395-8
                    [price] => 22.99
                )

        )

)
Assertion 3
Array
(
    [0] => WHAT
)
Assertion 4
Array
(
    [0] => WHAT
)
Assertion 5
Array
(
    [0] => Sayings of the Century
    [1] => Sword of Honour
    [2] => Moby Dick
    [3] => The Lord of the Rings
    [4] => HAH!
)
Assertion 6
Array
(
    [0] => Sayings of the Century
    [1] => Sword of Honour
    [2] => Moby Dick
    [3] => The Lord of the Rings
    [4] => HAH!
)
Assertion 7
Array
(
    [0] => WHAT
    [1] => Sayings of the Century
    [2] => Sword of Honour
    [3] => Moby Dick
    [4] => The Lord of the Rings
    [5] => HAH!
    [6] => Sayings of the Century
)
Assertion 8
Array
(
    [0] => WHAT
    [1] => Sayings of the Century
    [2] => Sword of Honour
    [3] => Moby Dick
    [4] => The Lord of the Rings
    [5] => HAH!
    [6] => Sayings of the Century
)
Assertion 9
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
Assertion 10
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
Assertion 11
Array
(
    [0] => reference
)
Assertion 12
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
Assertion 13
Array
(
    [0] => reference
)
Assertion 14
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
Assertion 15
Array
(
    [0] => reference
)
Assertion 16
Array
(
    [0] => Array
        (
            [0] => Array
                (
                    [category] => reference
                    [title] => Sayings of the Century
                    [price] => 8.95
                    [author] => Nigel Rees
                )

            [1] => Array
                (
                    [category] => fiction
                    [author] => Evelyn Waugh
                    [title] => Sword of Honour
                    [price] => 12.99
                )

            [2] => Array
                (
                    [category] => fiction
                    [author] => Herman Melville
                    [title] => Moby Dick
                    [isbn] => 0-553-21311-3
                    [price] => 8.99
                )

            [3] => Array
                (
                    [category] => fiction
                    [author] => J. R. R. Tolkien
                    [title] => The Lord of the Rings
                    [isbn] => 0-395-19395-8
                    [price] => 22.99
                )

        )

    [1] => Array
        (
            [color] => red
            [price] => 19.95
        )

    [2] => Array
        (
            [0] => 1
            [1] => 2
            [2] => 3
            [3] => Array
                (
                    [0] => 1
                )

            [4] => Array
                (
                    [price] => EMBEDDED
                )

        )

)
Assertion 17
Array
(
    [0] => Sword of Honour
    [1] => Moby Dick
)
Assertion 18
Array
(
    [0] => Sword of Honour
    [1] => The Lord of the Rings
)
Assertion 19
Array
(
    [0] => Sayings of the Century
    [1] => Sayings of the Century
    [2] => Sword of Honour
    [3] => Moby Dick
    [4] => The Lord of the Rings
)
Assertion 20
Array
(
    [0] => Array
        (
            [category] => fiction
            [author] => Evelyn Waugh
            [title] => Sword of Honour
            [price] => 12.99
        )

)
Assertion 21
Array
(
    [0] => Array
        (
            [category] => reference
            [title] => Sayings of the Century
            [price] => 8.95
            [author] => Nigel Rees
        )

    [1] => Array
        (
            [category] => fiction
            [author] => Evelyn Waugh
            [title] => Sword of Honour
            [price] => 12.99
        )

)
Assertion 22
Array
(
    [0] => Array
        (
            [category] => reference
            [title] => Sayings of the Century
            [price] => 8.95
            [author] => Nigel Rees
        )

    [1] => Array
        (
            [category] => fiction
            [author] => Herman Melville
            [title] => Moby Dick
            [isbn] => 0-553-21311-3
            [price] => 8.99
        )

)
Assertion 23
Array
(
    [0] => Array
        (
            [category] => fiction
            [author] => Herman Melville
            [title] => Moby Dick
            [isbn] => 0-553-21311-3
            [price] => 8.99
        )

)
Assertion 24
Array
(
    [0] => Array
        (
            [category] => fiction
            [author] => Evelyn Waugh
            [title] => Sword of Honour
            [price] => 12.99
        )

    [1] => Array
        (
            [category] => fiction
            [author] => Herman Melville
            [title] => Moby Dick
            [isbn] => 0-553-21311-3
            [price] => 8.99
        )

    [2] => Array
        (
            [category] => fiction
            [author] => J. R. R. Tolkien
            [title] => The Lord of the Rings
            [isbn] => 0-395-19395-8
            [price] => 22.99
        )

)
Assertion 25
Array
(
    [0] => Array
        (
            [category] => fiction
            [author] => Evelyn Waugh
            [title] => Sword of Honour
            [price] => 12.99
        )

    [1] => Array
        (
            [category] => fiction
            [author] => Herman Melville
            [title] => Moby Dick
            [isbn] => 0-553-21311-3
            [price] => 8.99
        )

)
Assertion 26
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
Assertion 27
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
Assertion 28
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
Assertion 29
Array
(
    [0] => Array
        (
            [category] => fiction
            [author] => Evelyn Waugh
            [title] => Sword of Honour
            [price] => 12.99
        )

)
Assertion 30
Array
(
    [0] => Array
        (
            [category] => fiction
            [author] => Evelyn Waugh
            [title] => Sword of Honour
            [price] => 12.99
        )

)
Assertion 31
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
Assertion 32
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
Assertion 33
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
Assertion 34
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
