<?php

namespace IPP\Student\Structure;

class MethodStructure
{
    public string $selector;
    /**
     * @var array<AssignStructure>
     */
    public array $assigns = [];
    /**
     * @var array<string>
     */
    public array $arguments = [];

    public function __construct(\DOMElement $method)
    {
        $this->selector = $method->getAttribute('selector');

        $block = $method->getElementsByTagName('block')->item(0);
        foreach ($block->childNodes as $child) {
            if ($child instanceof \DOMElement) {
                if ($child->nodeName === 'assign') {
                    $this->assigns[$child->getAttribute('order')] = new AssignStructure($child);
                } elseif ($child->nodeName === 'parameter') {
                    $this->arguments[$child->getAttribute('order')] = "{$child->getAttribute('name')}";
                }
            }
        }
        if (count($this->assigns) != 0) {
            ksort($this->assigns);
        }

        if (count($this->arguments) != 0) {
            ksort($this->arguments);
        }
    }
}
