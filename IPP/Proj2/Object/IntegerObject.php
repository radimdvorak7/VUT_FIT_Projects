<?php

namespace IPP\Student\Object;

use IPP\Student\CustomExceptions;
use IPP\Core\Interface\OutputWriter;
use IPP\Core\Interface\InputReader;

class IntegerObject extends BaseObject
{
    public $parent = "Object";

    public function __construct(InputReader $input, OutputWriter $output, int $value = 0)
    {
        parent::__construct($input, $output);
        $this->value = $value;
    }

    public function isInteger(): TrueObject|FalseObject
    {
        return TrueObject::getInstance();
    }

    public function equalTo(BaseObject $parameter): TrueObject|FalseObject
    {
        if ($parameter instanceof IntegerObject) {
            if ($this->value === $parameter->value) {
                return TrueObject::getInstance();
            } else {
                return FalseObject::getInstance();
            }
        }
        throw new CustomExceptions(52);
    }

    public function greaterThan(BaseObject $parameter): TrueObject|FalseObject
    {
        if ($parameter instanceof IntegerObject) {
            if ($this->value > $parameter->value) {
                return TrueObject::getInstance();
            } else {
                return FalseObject::getInstance();
            }
        }
        throw new CustomExceptions(52);
    }

    public function plus(BaseObject $parameter): IntegerObject
    {
        if ($parameter instanceof IntegerObject) {
            return new IntegerObject($this->input, $this->output, $this->value + $parameter->value);
        }
        throw new CustomExceptions(52);
    }

    public function minus(BaseObject $parameter): IntegerObject
    {
        if ($parameter instanceof IntegerObject) {
            return new IntegerObject($this->input, $this->output, $this->value - $parameter->value);
        }
        throw new CustomExceptions(52);
    }

    public function multiplyBy(BaseObject $parameter): IntegerObject
    {
        if ($parameter instanceof IntegerObject) {
            return new IntegerObject($this->input, $this->output, $this->value * $parameter->value);
        }
        throw new CustomExceptions(52);
    }

    public function divBy(BaseObject $parameter): IntegerObject
    {
        if ($parameter instanceof IntegerObject) {
            try {
                $result = intdiv($this->value, $parameter->value);
            } catch (\Throwable $th) {
                throw new CustomExceptions(53);
            }
            return new IntegerObject($this->input, $this->output, $result);
        }
        throw new CustomExceptions(52);
    }

    public function asString(): StringObject
    {
        return new StringObject($this->input, $this->output, $this->value);
    }

    public function asInteger(): IntegerObject
    {
        return $this;
    }
}
