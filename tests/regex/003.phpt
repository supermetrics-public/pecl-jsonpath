--TEST--
Test regex with broken regex pattern
--SKIPIF--
<?php if (!extension_loaded("jsonpath")) print "skip"; ?>
--FILE--
<?php

$data = [
  'test' => [
      [
        'id'      => 1,
        'val'     => 10,
        'val_str' => 'PHP is the web scripting language of choice',
      ],
    ],
];

$jsonPath = new JsonPath();
$result = $jsonPath->find($data, '$.test[?(@.val_str =~ "/invalid([a-]+/")]');

var_dump($result);
--EXPECTF--
Warning: JsonPath::find(): Compilation failed: missing closing parenthesis at offset 13 in %s

Fatal error: Uncaught RuntimeException: Invalid regex pattern `/invalid([a-]+/` in %s
Stack trace:
%s
%s
%s
