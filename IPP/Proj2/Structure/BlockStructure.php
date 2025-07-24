<?php

namespace IPP\Student\Structure;

class BlockStructure
{
    /**
     * @var string[] An array of arguments
     */
    public array $arguments = [];

    /**
     * @var AssignStructure[] An array of AssignStructures
     */
    public array $assigns;

    public function __construct(\DomElement $block)
    {

        foreach ($block->childNodes as $child) {
            if ($child instanceof \DOMElement) {
                if ($child->nodeName === 'assign') {
                    $this->assigns[$child->getAttribute('order')] = new AssignStructure($child);
                } elseif ($child->nodeName === 'parameter') {
                    $this->arguments[$child->getAttribute('order')] = "{$child->getAttribute('name')}";
                }
            }
        }

        ksort($this->assigns);
        if (count($this->arguments) != 0) {
            ksort($this->arguments);
        }
    }
    /**
     * @return AssignStructure[]
     */
    public function getBlock(): array
    {
        return $this->assigns;
    }

    /**
     * @return string[]
     */
    public function getArguments(): array
    {
        return $this->arguments;
    }
}
