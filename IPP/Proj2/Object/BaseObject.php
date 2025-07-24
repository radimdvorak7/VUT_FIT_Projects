<?php

namespace IPP\Student\Object;

use IPP\Core\Interface\InputReader;
use IPP\Core\Interface\OutputWriter;

class BaseObject
{
    public $value;
    public $attributes = [];
    public $methods = [];
    public $parent = null;

    public InputReader $input;
    public OutputWriter $output;

    public function __construct(InputReader $input, OutputWriter $output, $value = null)
    {
        $this->input = $input;
        $this->output = $output;
        $this->value = $value;
    }

    public function asString(): string| BaseObject
    {
        return new StringObject($this->input, $this->output, '\'\'');
    }

    public function identicalTo(BaseObject $object): TrueObject|FalseObject
    {
        if ($this === $object) {
            return TrueObject::getInstance();
        }
        return FalseObject::getInstance();
    }

    public function equalTo(BaseObject $parameter): TrueObject|FalseObject
    {

        return $this->identicalTo($parameter);
    }

    public function isInteger(): TrueObject|FalseObject
    {
        return FalseObject::getInstance();
    }

    public function isString(): TrueObject|FalseObject
    {
        return FalseObject::getInstance();
    }

    public function isBlock(): TrueObject|FalseObject
    {
        return FalseObject::getInstance();
    }

    public function isNil(): TrueObject|FalseObject
    {
        return FalseObject::getInstance();
    }
}
