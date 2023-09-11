# JsonPath-PHP - extension for PHP 7.4+

[![Build status](https://github.com/supermetrics-public/pecl-jsonpath/workflows/Build/badge.svg)](https://github.com/supermetrics-public/pecl-jsonpath/actions?query=workflow%3ABuild)
![Code coverage](https://img.shields.io/endpoint?url=https://gist.githubusercontent.com/crocodele/5ceb08845fe95635fc41af2f4c86c631/raw/pecl-jsonpath__main.json&labelColor=343940)

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

### Option 1: Build using PECL

```bash
$ pecl install jsonpath
```

For more instructions on how to use PECL, [see the PECL documentation](https://www.php.net/manual/en/install.pecl.php).

### Option 2: Build manually

Clone this repository to the directory of your choice, then run:

```bash
$ phpize
$ ./configure --enable-jsonpath
$ make
$ TEST_PHP_ARGS="-q" make test
$ sudo make install
```

### Enable the extension

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

use JsonPath\JsonPath;
use JsonPath\JsonPathException;

$jsonPath = new JsonPath();

try {
    // Query a data array using a JSONPath expression string.
    // Returns an array of matching elements, or false if nothing was found.
    $result = $jsonPath->find($data, $selector);
} catch (JsonPathException $exception) {
    echo $exception->getMessage(); // Usually a syntax error in the selector
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

$jsonPath = new \JsonPath\JsonPath();

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

echo "Example 6 - The title of the last book:\n";
echo json_encode($jsonPath->find($data, "$..book[-1].title"), JSON_PRETTY_PRINT);
echo "\n\n";
/*
[
    "The Lord of the Rings"
]
*/

echo "Example 7 - Lookup by ISBN:\n";
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

echo "Example 8 - Lookup by category and price:\n";
echo json_encode($jsonPath->find($data, "$.store.book[?(@.category == 'fiction' && @.price <= 15.00)]"), JSON_PRETTY_PRINT);
echo "\n\n";
/*
[
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
    }
]
*/

echo "Example 9 - Lookup by author using regular expression:\n";
echo json_encode($jsonPath->find($data, "$.store.book[?(@.author =~ /Tolkien$/)]"), JSON_PRETTY_PRINT);
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

echo "Example 10 - The titles of books without an ISBN:\n";
echo json_encode($jsonPath->find($data, "$.store.book[?(!@.isbn)].title"), JSON_PRETTY_PRINT);
echo "\n\n";
/*
[
    "Sayings of the Century",
    "Sword of Honour"
]
*/
```

## JSONPath expression syntax

| Notation                     | Description                                                                                                                                                                                                                                                                                                                                                                                                                                                                            |
|------------------------------|----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------|
| `$`                          | References the root element.                                                                                                                                                                                                                                                                                                                                                                                                                                                           |
| `@`                          | References the current element. Often used in filter expressions.                                                                                                                                                                                                                                                                                                                                                                                                                      |
| `.property`                  | Picks the child element with the given property name, using dot notation.                                                                                                                                                                                                                                                                                                                                                                                                              |
| `["property"]`               | Picks the child element with the given property name, using bracket notation. Use this when the property name contains special characters.                                                                                                                                                                                                                                                                                                                                             |
| `[n]`                        | Picks the array element at index _n_. Use a negative index to count from the end of the array.                                                                                                                                                                                                                                                                                                                                                                                         |
| `[start:end:step]`           | Picks every _step_ array elements from index _start_ to index _end_, end exclusive. Use negative indexes to count from the end of the array, `-1` denoting the last item. Examples:<br/><ul><li>`$.list[0:3]` - Takes the first three elements</li><li>`$.list[1:]` - Leaves out the first element</li><li>`$.list[1:-1]` - Leaves out the first and last elements</li><li>`$.list[::2]` - Takes every second element</li></ul>                                                        |
| `["property1", "property2"]` | Picks the given properties from an array.                                                                                                                                                                                                                                                                                                                                                                                                                                              |
| `*`                          | Picks all elements in an array.                                                                                                                                                                                                                                                                                                                                                                                                                                                        |
| `..property`                 | Recursively picks all elements with the given property name.                                                                                                                                                                                                                                                                                                                                                                                                                           |
| `[?(expression)]`            | Picks all elements in an array that match the given filter. Use `@` to reference the current element. Supported filter operators: <ul><li>`==`: Equals</li><li>`!=`: Does not equal</li><li>`=~ /REGEX/`: Matches the given regular expression _REGEX_</li><li>`>`: Is greater than</li><li>`>=`: Is greater than or equal to</li><li>`<`: Is less than</li><li>`<=`: Is less than or equal to</li><li>`!`: Negation</li><li>`&&`: AND</li><li><code>&#124;&#124;</code>: OR</li></ul> |

Also see the [examples](#examples).

## Performance benchmarks

![Chart showing a performance comparison between JsonPath-PHP, native PHP (JsonPath-PHP sometimes faster, sometimes slower), and the fastest tested PHP library (JsonPath-PHP always faster)](benchmark.png)

The numbers in the chart were obtained when running the benchmark suite in PHP 8.1 with OPcache and JIT enabled.

If you want to run benchmarks yourself, have a look at the [jsonpath-benchmark](https://github.com/supermetrics-public/jsonpath-benchmark) repo.

## License

JsonPath-PHP is open-sourced software licensed under the [PHP License 3.01](https://opensource.org/licenses/PHP-3.01).

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

## Code Quality Tooling

Please run the code formatter and linter (and fix any issues reported by the linter) before filing a pull request.

You may check the code format and linter automatically before committing by setting up a pre-commit hook:

```bash
make setup-pre-commit
```

### Formatting

Code formatting requires [clang-format](https://clang.llvm.org/docs/index.html).

```bash
make format-code
```

### Linting

Linting requires [cppcheck](http://cppcheck.sourceforge.net/).

```bash
make lint-code
```

## Contributors

JsonPath-PHP is created by [Mike Kaminski](https://github.com/mkaminski1988) and maintained by 
[Supermetrics](https://github.com/supermetrics-public) and Mike Kaminski.

Found a bug, or missing some feature? [Raise an issue](https://github.com/supermetrics-public/pecl-jsonpath/issues) or
[submit a pull request](https://github.com/supermetrics-public/pecl-jsonpath/pulls).
