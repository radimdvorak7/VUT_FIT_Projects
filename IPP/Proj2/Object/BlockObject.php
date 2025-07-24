<?php

namespace IPP\Student\Object;

use IPP\Student\Structure\AssignStructure;

class BlockObject extends BaseObject
{
    public $parent = "Object";
    /**
     * @var BaseObject[] An array of BaseObject instances
     */
    public ?array $parameters;

    /**
     * @var AssignStructure[] An array of BaseObject instances
     */
    public array $assigns;

    public function __construct(?array $parameters, array $assigns)
    {

        $this->parameters = $parameters;
        $this->assigns = $assigns;
    }
}
