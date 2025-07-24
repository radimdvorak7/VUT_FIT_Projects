<?php

namespace IPP\Student\Object;

use IPP\Core\Interface\InputReader;
use IPP\Core\Interface\OutputWriter;
use IPP\Student\CustomExceptions;

class StringObject extends BaseObject
{
    public $value;
    public $parent = "Object";


    public function __construct(InputReader $input, OutputWriter $output, string $value = "")
    {
        parent::__construct($input, $output);
        $this->value = $value;
    }

    public function isString(): TrueObject|FalseObject
    {
        return TrueObject::getInstance();
    }

    public function read(): StringObject
    {
        return new StringObject($this->input, $this->output, $this->input->readString());
    }

    public function print(): StringObject
    {
        $this->output->writeString(stripcslashes($this->value));
        return $this;
    }

    public function equalTo(BaseObject $parameter): TrueObject|FalseObject
    {
        if ($parameter instanceof StringObject) {
            if ($this->value === $parameter->value) {
                return TrueObject::getInstance();
            } else {
                return FalseObject::getInstance();
            }
        }
        throw new CustomExceptions(52);
    }

    public function asString(): StringObject
    {
        return $this;
    }

    public function asInteger(): IntegerObject| NilObject
    {
        $value = filter_var($this->value, FILTER_VALIDATE_INT);

        if ($value == false) {
            return NilObject::getInstance();
        }
        return new IntegerObject($this->input, $this->output, $value);
    }

    public function concatenateWith(BaseObject $parameter): StringObject|NilObject
    {
        if ($parameter instanceof StringObject) {
            return new StringObject($this->input, $this->output, $this->value . $parameter->value);
        }
        return NilObject::getInstance();
    }
}
