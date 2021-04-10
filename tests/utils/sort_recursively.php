<?php

/**
 * Comparison function for custom sort. Tries to achieve a canonical sort order across PHP versions
 * by sorting primarily by type, secondarily by value.
 * @param mixed $a
 * @param mixed $b
 *
 * @return int
 */
function byTypeAndValue($a, $b): int
{
    if (gettype($a) !== gettype($b)) {
        return gettype($a) <=> gettype($b);
    } else {
        return $a <=> $b;
    }
}

/**
 * Sort an array by its values, recursively
 * @param array &$array
 */
function sortRecursively(array &$array): void
{
    // Sequential array, re-index after sorting
    if (array_keys($array) === range(0, count($array) - 1)) {
        usort($array, 'byTypeAndValue');
    }
    // Associative array, maintain keys
    else {
        uasort($array, 'byTypeAndValue');
    }
    
    foreach ($array as &$value) {
        if (is_array($value)) {
            sortRecursively($value);
        }
    }
}
