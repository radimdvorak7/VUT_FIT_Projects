<?php

namespace IPP\Student\Structure;

class LiteralStructure
{
    public string $class;
    public string $value;

    public function __construct(\DOMElement $literal)
    {
        $this->class = $literal->getAttribute('class');
        $this->value = $literal->getAttribute('value');
    }
}
