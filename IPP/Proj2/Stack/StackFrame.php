<?php

namespace IPP\Student\Stack;

use IPP\Student\CustomExceptions;
use IPP\Student\Object\BaseObject;

class StackFrame
{
    public string $selector;
    /**
     * @var array<BaseObject>
     */
    public $parameters = [];
    /**
     * @var array<BaseObject>
     */
    public $objects = [];

    /**
     * @param string $selector
     * @param BaseObject[] $parameters
     * @param BaseObject[]  $objects
     */
    public function __construct(string $selector, array $parameters, array $objects)
    {
        $this->selector = $selector;
        $this->parameters = $parameters;
        $this->objects = $objects;
    }

    public function searchObjects(string $className): array
    {
        $result = [];
        foreach ($this->objects as $object) {
            if ($object instanceof $className) {
                $result[] = $object;
            }
        }
        return $result;
    }

    public function appendObject(string $name, BaseObject $object): void
    {
        $this->objects["$name"] = $object;
    }

    public function findObject(string $name)
    {
        if (!array_key_exists($name, $this->objects)) {
            throw new CustomExceptions(32);
        } else {
            return $this->objects[$name];
        }
    }
}
