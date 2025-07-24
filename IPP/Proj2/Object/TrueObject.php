<?php

namespace IPP\Student\Object;

class TrueObject extends BaseObject
{
    public $value = 'true';
    public $parent = "Object";
    private static ?TrueObject $instance = null;

    private function __construct()
    {
    }

    public static function getInstance(): TrueObject
    {
        if (self::$instance === null) {
            self::$instance = new TrueObject();
        }
        return self::$instance;
    }

    public function not(): FalseObject
    {
        return FalseObject::getInstance();
    }

    public function and(TrueObject|FalseObject $parameter): TrueObject|FalseObject
    {
        if ($parameter instanceof TrueObject) {
            return TrueObject::getInstance();
        }
        return FalseObject::getInstance();
    }

    public function ifTrueifFalse()
    {
        return TrueObject::getInstance();
    }
}
