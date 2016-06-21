# JsonPath-PHP - extension for PHP 5.x/7.x

JsonPath is a language for querying and extracting data from JSON, similar to 
XPath. This implementation is inspired by [Stefan Goessner's original implementation](http://goessner.net/articles/JsonPath/) 
and intended to be a fast(er) replacement for [Peekmo's PHP implementation](https://github.com/Peekmo/JsonPath).

## Intended Audience

This extension is ideal for projects that depend heavily on JsonPath and could benefit from a native C implementation. Other 
JsonPath implementations depend heavily on `eval()` and regular expressions, while JsonPath-PHP is a more efficient lexer/parser.
The tradeoff for increased performance is the overhead of adding this extension, so we recommend checking out more convenient
PHP implementations like [this](https://github.com/Peekmo/JsonPath), [this](https://github.com/FlowCommunications/JSONPath)
and [this](https://github.com/Skyscanner/JsonPath-PHP).

## Installation

The only available installation option available is a [manual build process](http://php.net/manual/en/install.pecl.phpize.php). PECL and
binaries are forthcoming.

```bash
$ phpize
$ ./configure --with-jsonpath
$ make
$ TEST_PHP_ARGS="-q" make test
$ sudo make install
```

Make sure PHP knows about the extension.

```bash
$ sudo touch /etc/php.d/jsonpath.ini
$ echo "extension=jsonpath.so" | sudo tee /etc/php.d/jsonpath.ini
$ sudo service php-fpm restart
```

## Usage

```php
<?php

/**
 * Query array using JsonPath string. Return array of matching elements or false if nothing was found.
 * 
 * @return array|false
 */
 function path_lookup(array $document, string $jsonPath) {
  // -- 
 }
```

## Examples

```php
<?php

$json = <<<JSON
{ "store": {
    "book": [ 
      { "category": "reference",
        "author": "Nigel Rees",
        "title": "Sayings of the Century",
        "price": 8.95
      },
      { "category": "fiction",
        "author": "Evelyn Waugh",
        "title": "Sword of Honour",
        "price": 12.99
      },
      { "category": "fiction",
        "author": "Herman Melville",
        "title": "Moby Dick",
        "isbn": "0-553-21311-3",
        "price": 8.99
      },
      { "category": "fiction",
        "author": "J. R. R. Tolkien",
        "title": "The Lord of the Rings",
        "isbn": "0-395-19395-8",
        "price": 22.99
      }
    ],
    "bicycle": {
      "color": "red",
      "price": 19.95
    }
  }
}
JSON;

$data = json_decode($json, true);

echo "Example 1: - The authors of all books in the store: \n";
echo json_encode(path_lookup($data, "$.store.book[*].author"));
echo "\n\n";
/*
 ["Nigel Rees","Evelyn Waugh","Herman Melville","J. R. R. Tolkien"]
*/

echo "Example 2 - All Authors:\n";
echo json_encode(path_lookup($data, "$..author"), JSON_PRETTY_PRINT);
echo "\n\n";
/*
[
    "Nigel Rees",
    "Evelyn Waugh",
    "Herman Melville",
    "J. R. R. Tolkien"
]
*/

echo "Example 3 - All things in the store:\n";
echo json_encode(path_lookup($data, "$.store.*"), JSON_PRETTY_PRINT);
echo "\n\n";
/*
[
    [
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
    ],
    {
        "color": "red",
        "price": 19.95
    }
]
*/

echo "Example 4 - The price of everything in the store:\n";
echo json_encode(path_lookup($data, "$.store..price"), JSON_PRETTY_PRINT);
echo "\n\n";
/*
[
    8.95,
    12.99,
    8.99,
    22.99,
    19.95
]
*/

echo "Example 5 - The third book:\n";
echo json_encode(path_lookup($data, "$..book[2]"), JSON_PRETTY_PRINT);
echo "\n\n";
/*
[
    {
        "category": "fiction",
        "author": "Herman Melville",
        "title": "Moby Dick",
        "isbn": "0-553-21311-3",
        "price": 8.99
    }
]
*/

echo "Example 6 - Lookup by ISBN:\n";
echo json_encode(path_lookup($data, "$.store.book[?(@.isbn == '0-395-19395-8')]"), JSON_PRETTY_PRINT);
echo "\n\n";
/*
[
    {
        "category": "fiction",
        "author": "J. R. R. Tolkien",
        "title": "The Lord of the Rings",
        "isbn": "0-395-19395-8",
        "price": 22.99
    }
]
*/
```
## License

JsonPath-PHP is open-sourced software licensed under the [MIT license](http://opensource.org/licenses/MIT).
