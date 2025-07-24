<?php

namespace IPP\Student\Structure;

class ClassStructure
{
    public string $name;
    public string $parent;

    /**
     * @var array<MethodStructure>
     */
    public array $methods = [];


    public function __construct(\DOMElement $class)
    {
        $this->name = $class->getAttribute('name');
        $this->parent = $class->getAttribute('parent');
        foreach ($class->childNodes as $method) {
            if ($method instanceof \DOMElement) {
                $methodStructure = new MethodStructure($method);
                $this->addMethod($methodStructure);
            }
        }
    }

    public function addMethod(MethodStructure $method): void
    {
        $this->methods[$method->selector] = $method;
    }

    public function getMethod(string $selector): ?MethodStructure
    {
        return $this->methods[$selector] ?? null;
    }

    public function hasMethod(string $selector): bool
    {
        return isset($this->methods[$selector]);
    }
}
