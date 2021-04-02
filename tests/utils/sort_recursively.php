<?php

function sortRecursively(&$array): void
{
    // Sequential array, re-index after sorting
    if (array_keys($array) === range(0, count($array) - 1)) {
        sort($array);
    }
    // Associative, maintain keys
    else {
        asort($array);
    }
    
    foreach ($array as &$value) {
        if (is_array($value)) {
            sortRecursively($value);
        }
    }
}
