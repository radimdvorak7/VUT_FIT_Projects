<?php

namespace IPP\Student\Object;

class NilObject extends BaseObject
{
    public $value = 'nil';
    public $parent = "Object";

    private static ?NilObject $instance = null;

    private function __construct()
    {
        parent::__construct($this->input, $this->output);
    }

    public static function getInstance(): NilObject
    {
        if (self::$instance === null) {
            self::$instance = new NilObject();
        }
        return self::$instance;
    }

    public function asString(): StringObject
    {
        return new StringObject($this->input, $this->output, $this->value);
    }

    public function isNil(): TrueObject|FalseObject
    {
        return TrueObject::getInstance();
    }
}
