<?php

/**
 * @generate-class-entries
 * @generate-legacy-arginfo
 */

namespace JsonPath;

class JsonPath
{
    /**
     * @param array $data
     * @param string $expression
     *
     * @return array|bool
     *
     * @throws JsonPathException
     */
    public function find(array $data, string $expression): array|bool;
}

class JsonPathException extends \RuntimeException
{
}