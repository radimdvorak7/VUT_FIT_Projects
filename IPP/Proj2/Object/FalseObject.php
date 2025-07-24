<?php

namespace IPP\Student\Object;

class FalseObject extends BaseObject
{
    public $value = 'false';
    public $parent = "Object";
    private static ?FalseObject $instance = null;

    private function __construct()
    {
    }

    public static function getInstance(): FalseObject
    {
        if (self::$instance === null) {
            self::$instance = new FalseObject();
        }
        return self::$instance;
    }

    public function not(): TrueObject
    {
        return TrueObject::getInstance();
    }

    public function and(TrueObject|FalseObject $_parameter): TrueObject|FalseObject
    {
        return FalseObject::getInstance();
    }

    public function ifTrueifFalse()
    {
        return FalseObject::getInstance();
    }
}
