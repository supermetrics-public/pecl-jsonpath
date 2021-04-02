<?php

/**
 * Sort an array by its values, recursively
 * @param array &$array
 */
function sortRecursively(array &$array): void
{
    // Sequential array, re-index after sorting
    if (array_keys($array) === range(0, count($array) - 1)) {
        sort($array);
    }
    // Associative array, maintain keys
    else {
        asort($array);
    }
    
    foreach ($array as &$value) {
        if (is_array($value)) {
            sortRecursively($value);
        }
    }
}
