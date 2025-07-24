<?php

namespace IPP\Student\Structure;

use IPP\Student\Object\BaseObject;

class AssignStructure
{
    public ExprStructure $expr;
    public string $result;

    public function __construct(\DOMElement $assign)
    {

        $this->result = $assign->getAttribute('name');

        foreach ($assign->childNodes as $child) {
            if ($child instanceof \DOMElement) {
                if ($child->nodeName === 'var') {
                    $this->result = $child->getAttribute('name');
                } elseif ($child->nodeName === 'expr') {
                    $this->expr = new ExprStructure($child);
                }
            }
        }
    }
}
