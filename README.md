# JsonPath-PHP - extension for PHP 7.4+

[![Build status](https://github.com/supermetrics/php-ext-jsonpath/workflows/Build/badge.svg)](https://github.com/supermetrics/php-ext-jsonpath/actions?query=workflow%3ABuild)
![Code coverage](https://img.shields.io/endpoint?url=https://gist.githubusercontent.com/crocodele/5ceb08845fe95635fc41af2f4c86c631/raw/php-ext-jsonpath__main.json&labelColor=343940)

JSONPath is a language for querying and extracting data from JSON, similar to XPath for XML.

This PHP extension for JSONPath is inspired by [Stefan Goessner's original implementation](https://goessner.net/articles/JsonPath/) 
and is intended to be a fast(er) replacement for JSONPath libraries written in PHP.

## Intended audience

This extension is ideal for projects that depend heavily on JSONPath and could benefit from a native C implementation. Other 
JSONPath implementations often rely on `eval()` and regular expressions, while JsonPath-PHP is a more efficient lexer/parser.
Extracting the data in C rather than in PHP results in great performance. In fact, JsonPath-PHP is often faster than manually 
traversing the array in PHP and picking the matching element(s).

The tradeoff for increased performance is the overhead of adding this extension to your PHP installation. If you're running 
your PHP application in a shared environment where you can't install PHP extensions, we recommend checking out more convenient 
PHP implementations like [this](https://github.com/SoftCreatR/JSONPath), [this](https://github.com/Galbar/JsonPath-PHP), and 
[this](https://github.com/Peekmo/JsonPath). Or switching to an environment that you have more control over. 😉

## Installation

The only installation option available for now is a [manual build process](https://www.php.net/manual/en/install.pecl.phpize.php).
PECL and binaries are forthcoming.

```bash
$ phpize
$ ./configure --enable-jsonpath
$ make
$ TEST_PHP_ARGS="-q" make test
$ sudo make install
```

Make sure PHP knows about the extension:

```bash
$ sudo touch /etc/php.d/jsonpath.ini
$ echo "extension=jsonpath.so" | sudo tee /etc/php.d/jsonpath.ini
$ php -m | grep jsonpath
```

If you're running PHP with the PHP-FPM service, restart it for the changes to take effect:

```bash
$ sudo service php-fpm restart
```

## Usage

```php
<?php

$jsonPath = new JsonPath();

// Query a data array using a JSONPath expression string.
// Returns an array of matching elements, or false if nothing was found.
$result = $jsonPath->find($data, $selector);

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

$jsonPath = new JsonPath();

echo "Example 1: - The authors of all books in the store: \n";
echo json_encode($jsonPath->find($data, "$.store.book[*].author"), JSON_PRETTY_PRINT);
echo "\n\n";
/*
[
    "Nigel Rees",
    "Evelyn Waugh",
    "Herman Melville",
    "J. R. R. Tolkien"
]
*/

echo "Example 2 - All authors:\n";
echo json_encode($jsonPath->find($data, "$..author"), JSON_PRETTY_PRINT);
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
echo json_encode($jsonPath->find($data, "$.store.*"), JSON_PRETTY_PRINT);
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
echo json_encode($jsonPath->find($data, "$.store..price"), JSON_PRETTY_PRINT);
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
echo json_encode($jsonPath->find($data, "$..book[2]"), JSON_PRETTY_PRINT);
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
echo json_encode($jsonPath->find($data, "$.store.book[?(@.isbn == '0-395-19395-8')]"), JSON_PRETTY_PRINT);
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

## JSONPath expression syntax

To be added.

## Performance benchmarks

To be added.

## License

JsonPath-PHP is open-sourced software licensed under the [MIT license](https://opensource.org/licenses/MIT).

## Tests

The unit tests utilize PHP's core and extension testing framework. The recommended way to run the tests is:

```bash
$ TEST_PHP_ARGS="-q" make test
```

JsonPath-PHP implements the [JSONPath Comparison test suite](https://github.com/cburgmer/json-path-comparison).
These tests reside in the `tests/comparison_*` directories.

To generate a code coverage report, install lcov and build the extension with the special `--enable-code-coverage`
flag, before running the tests and processing the code coverage output:

```bash
$ ./configure --enable-jsonpath --enable-code-coverage
$ make clean
$ TEST_PHP_ARGS="-q" make test
$ make lcov
```

You can then find a coverage report in the `lcov_html/` directory.

## Contributors

JsonPath-PHP is created by [Mike Kaminski](https://github.com/mkaminski1988) and maintained by 
[Supermetrics](https://github.com/supermetrics) and Mike Kaminski.

Found a bug, or missing some feature? [Raise an issue](https://github.com/supermetrics/php-ext-jsonpath/issues) or
[submit a pull request](https://github.com/supermetrics/php-ext-jsonpath/pulls).
