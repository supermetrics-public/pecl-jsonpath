--TEST--
Test typical use cases
--SKIPIF--
<?php if (!extension_loaded("jsonpath")) print "skip"; ?>
--FILE--
<?php

$data = [
    'level1' =>
        [
            'level2a' =>
                [
                    'level3' =>
                        [
                            'level4' =>
                                ['elem1', 'elem2'],
                        ]
                ],
            'level2b' =>
                [
                    'level3' =>
                        [
                            'level4' =>
                                ['elem1', 'elem2'],
                        ]
                ],
        ],
];

echo "Assertion 1\n";
print_r(path_lookup($data, '$.level1.*.level3.level4[0]'));

echo "Assertion 2\n";
var_dump(path_lookup($data, '$.level1.*.level3.level10[10]'));
?>
--EXPECT--
Assertion 1
Array
(
    [0] => elem1
    [1] => elem1
)
Assertion 2
bool(false)
