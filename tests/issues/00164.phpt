--TEST--
Test escaped quotes in selectors and string literals
--DESCRIPTION--
https://github.com/supermetrics-public/pecl-jsonpath/issues/164
--SKIPIF--
<?php if (!extension_loaded("jsonpath")) print "skip"; ?>
--FILE--
<?php

$obj = [
    'store' => [
        'book' => [
            [
                'category' => 'reference',
                'title' => 'Sayings of the Century',
                "the 'author'" => "Nigel 'Rees'",
            ],
            [
                'category' => 'fiction',
                'title' => 'Sword of Honour',
                'the "author"' => 'Evelyn "Waugh"',
            ],
            [
                'category' => 'fiction',
                'title' => 'Moby Dick',
                "the 'author'" => 'Herman Melville',
            ],
            [
                'category' => 'fiction',
                'title' => 'The Lord of the Rings',
                'the "author"' => 'J. R. R. Tolkien',
            ]
        ]
    ]
];

$jsonPath = new \JsonPath\JsonPath();

print_r($jsonPath->find($obj, '$.store.book[?(@["the \"author\""] == "Evelyn \"Waugh\"" || '."@['the \'author\''] == 'Nigel \'Rees\'')]"));

--EXPECT--
Array
(
    [0] => Array
        (
            [category] => reference
            [title] => Sayings of the Century
            [the 'author'] => Nigel 'Rees'
        )

    [1] => Array
        (
            [category] => fiction
            [title] => Sword of Honour
            [the "author"] => Evelyn "Waugh"
        )

)