--TEST--
Test typical use cases
--SKIPIF--
<?php if (!extension_loaded("jsonpath")) print "skip"; ?>
--FILE--
<?php

$jsonPath = new \JsonPath\JsonPath();

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
print_r($jsonPath->find($data, '$.level1.*.level3.level4[0]'));

echo "Assertion 2\n";
var_dump($jsonPath->find($data, '$.level1.*.level3.level10[10]'));

$data = [
  "level1"=> [
    "level2"=> [
      "level3"=> [
        [
          "name"=> "val1",
          "level4"=> [
            [
              "level5"=> [
                [
                  "level6"=> [
                    [
                      "id"=> "val2"
                    ]
                  ]
                ]
              ]
            ]
          ]
        ]
      ]
    ]
  ]
];

echo "Assertion 3\n";
var_dump($jsonPath->find($data, "$.level1.level2.level3[?(@['name'] == 'val1')].level4[*].level5[*]"));
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
Assertion 3
array(1) {
  [0]=>
  array(1) {
    ["level6"]=>
    array(1) {
      [0]=>
      array(1) {
        ["id"]=>
        string(4) "val2"
      }
    }
  }
}
