<?php

namespace IPP\Student\Stack;

use IPP\Student\Object\BaseObject;
use IPP\Student\Object\FalseObject;
use IPP\Student\Object\NilObject;
use IPP\Student\Object\TrueObject;

class Stack
{
    /**
     * @var StackFrame[]
     */
    private array $stack;

    public function __construct()
    {
        $this->push(new StackFrame("Main", [], []));
    }

    public function push(StackFrame $frame): void
    {

        $this->stack[] = $frame;
    }

    public function pop(): StackFrame
    {

        return array_pop($this->stack);
    }

    public function peek(): StackFrame
    {
        $peek = end($this->stack);

        return $peek;
    }

    public function appendToStackFrame(string $name, BaseObject $object): void
    {
        $currentFrame = $this->peek();
        $currentFrame->appendObject($name, $object);
    }

    public function findObject(string $name): ?BaseObject
    {
        return $this->peek()->findObject($name);
    }
}
