<?php

/**
 * @generate-class-entries
 */

namespace JsonPath;

class JsonPath
{
    /**
     * @throws JsonPathException
     */
    public function find(array $data, string $expression): array|false;
}

class JsonPathException extends \RuntimeException
{
}