<?php

namespace IPP\Student\Object;

use IPP\Core\Interface\InputReader;
use IPP\Core\Interface\OutputWriter;

class ClassObject extends BaseObject
{
    public $parent;
    public $name;
    public $methods = [];
    public $properties = [];


    public function __construct(InputReader $input, OutputWriter $output, string $name)
    {
        parent::__construct($input, $output);
        $this->name = $name;
    }
}
