<?php

namespace IPP\Student\Structure;

class ExprStructure
{
    public $base = null;
    public $baseType = null;

    public function __construct(\DomElement $expr)
    {

        if ($expr->hasChildNodes()) {
            foreach ($expr->childNodes as $child) {
                if ($child instanceof \DomElement) {
                    if ($child->nodeName === 'literal') {
                        // Create literal object
                        $this->base = new LiteralStructure($child);
                        $this->baseType = $this->base->value;
                    } elseif ($child->nodeName === 'var') {
                        // Set variable and assign value
                        $this->baseType = "var";
                        $this->base = $child->getAttributeNode("name")->nodeValue;
                    } elseif ($child->nodeName === 'expr') {
                        // Create new ExprStructure object for nested expressions
                        $this->baseType = "expr";
                        $this->base = new ExprStructure($child);
                    } elseif ($child->nodeName === 'send') {
                        // Create new SendStructure object
                        $this->baseType = "send";
                        $this->base = new SendStructure($child);
                    } elseif ($child->nodeName === 'block') {
                        // Create new BlockStructure object
                        $this->baseType = "block";
                        $this->base = new BlockStructure($child);
                    }
                }
            }
        }
    }


    public function getReceiverType(): string
    {
        while ($this->base != null) {
            if ($this->base instanceof SendStructure) {
                return $this->base->getReceiverType();
            } elseif ($this->base instanceof ExprStructure) {
                return $this->base->getReceiverType();
            } elseif ($this->base instanceof LiteralStructure) {
                return $this->base->class;
            } else {
                return $this->baseType;
            }
        }
        return $this->baseType;
    }
}
