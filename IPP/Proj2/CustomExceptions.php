<?php

namespace IPP\Student;

use IPP\Core\Exception\IPPException;
use Throwable;

class CustomExceptions extends IPPException
{
    public function __construct(int $code, ?Throwable $previous = null)
    {
        $message = self::getErrorMessage($code);
        parent::__construct($message, $code, $previous, false);
    }

    private static function getErrorMessage(int $code): string
    {
        switch ($code) {
            case 31:
                return "Semantic error - missing class Main and method run.";
            case 32:
                return "Semantic error - undefined variable";
            case 41:
                return "Incorrect XML format in the input file (file is not well-formed).";
            case 42:
                return "Unexpected XML structure.";
            case 51:
                return "Runtime error - do-not-understand error.";
            case 52:
                return "Runtime error - type error.";
            case 53:
                return "Runtime error - value (div by zero) error.";
            case 99:
                return "Internal error.";
            default:
                return "Error.";
        }
    }
}
