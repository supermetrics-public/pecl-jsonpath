<?php

require 'vendor/autoload.php';

/**
 * Created by PhpStorm.
 * User: mike
 * Date: 4/6/16
 * Time: 11:47 PM
 */
class TestJsonPath extends PHPUnit_Framework_TestCase
{

    private static $obj;

    private static $jsonPath;

    public static function setUpBeforeClass()
    {
        $json = <<<JSON
{
    "store": {
        "book": [
            {
                "category": "reference",
                "title": "Sayings of the Century",
                "price": 8.95,
                "author": "Nigel Rees"
            },
            {
                "category": "fiction",
                "author": "Evelyn Waugh",
                "title": "Sword of Honour",
                "price": 12.99
            },
            {
                "category": "fiction",
                "author": "Herman Melville",
                "title": "Moby Dick",
                "isbn": "0-553-21311-3",
                "price": 8.99
            },
            {
                "category": "fiction",
                "author": "J. R. R. Tolkien",
                "title": "The Lord of the Rings",
                "isbn": "0-395-19395-8",
                "price": 22.99
            }
        ],
        "bicycle": {
            "color": "red",
            "price": 19.95
        },
        "price": [1, 2, 3, [1], {"price": "EMBEDDED"}]
    },
    "expensive": 10,
    "title": "WHAT",
    "hello": {
        "store" : [
            {
                "title": "HAH!"
            }
        ]
    },
    "book": [
        {
            "category": "reference",
            "author": "Nigel Rees",
            "title": "Sayings of the Century",
            "price": 8.95
        },
        {
            "category": "fiction",
            "author": "Evelyn Waugh",
            "title": "Sword of Honour",
            "price": 12.99
        },
        {
            "category": "fiction",
            "author": "Herman Melville",
            "title": "Moby Dick",
            "isbn": "0-553-21311-3",
            "price": 8.99
        },
        {
            "category": "fiction",
            "author": "J. R. R. Tolkien",
            "title": "The Lord of the Rings",
            "isbn": "0-395-19395-8",
            "price": 22.99
        }
    ]
}
JSON;

        static::$obj = json_decode($json, true);
        static::$jsonPath = new \Peekmo\JsonPath\JsonPath();
    }

    public function test1()
    {
        $this->compare('$.store.book');
    }

    public function test2()
    {
        $this->compare("$['store']['book']");
    }

    public function test3()
    {
        $this->compare('$.title');
    }

    public function test4()
    {
        $this->compare("$['title']");
    }

    public function test5()
    {
        $this->compare('$..store..title');
    }

    public function test6()
    {
        $this->compare("$..['store']..['title']");
    }
//
    public function test7()
    {
        $this->compare('$..title');
    }

    public function test8()
    {
        $this->compare("$..['title']");
    }

    public function test9()
    {
        $this->compare('$.book[0,3]');
    }

    public function test10()
    {
        $this->compare("$['book'][0,3]");
    }

    public function test11()
    {
        $this->compare('$.book[0,1,2].category');
    }

    public function test12()
    {
        $this->compare('$.book[0:2]');
    }

    public function test13()
    {
        $this->compare('$.book[0:2].category');
    }

    public function test14()
    {
        $this->compare('$.book[*]');
    }

    public function test15()
    {
        $this->compare('$.book[*].category');
    }

    public function test16()
    {
        $this->compare('$.store.*');
    }

    public function test17()
    {
        $this->compare('$..book[1:3].title');
    }

    public function test18()
    {
        $this->compare('$..book[1,3].title');
    }

    public function test19()
    {
        $this->compare('$..book[*].title');
    }

    public function test20()
    {
        $this->compare('$.store.book[?(@.author == "Evelyn Waugh")]');
    }

    public function test21()
    {
        $this->compare('$.store.book[?(@.author == "Evelyn Waugh" || @.author == "Nigel Rees")]');
    }

    public function test22()
    {
        $this->compare('$.store.book[?(@.author == "Nigel Rees" || @.title == "Moby Dick")]');
    }

    public function test23()
    {
        $this->compare('$.store.book[?(@.author == "Herman Melville" && @.title == "Moby Dick")]');
    }

    public function test24()
    {
        $this->compare('$.store.book[?(@.category == "fiction")]'); //Returns 3 of a kind
    }

    public function test25()
    {
        $this->compare('$.store.book[?(@.author == "Herman Melville" && @.title == "Moby Dick" || @.author == "Herman Melville")]');
    }

    public function compare($path)
    {
        $this->assertSame(
            static::$jsonPath->jsonPath(static::$obj, $path),
            path_lookup(static::$obj, $path)
        );
    }
}
