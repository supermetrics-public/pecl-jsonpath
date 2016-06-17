--TEST--
Test typical use cases
--SKIPIF--
<?php if (!extension_loaded("jsonpath")) print "skip"; ?>
--FILE--
<?php

$data = [
    'level1' =>
        [
            'level2' =>
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

?>
--EXPECT--
Assertion 1
Array
(
    [0] => elem1
)