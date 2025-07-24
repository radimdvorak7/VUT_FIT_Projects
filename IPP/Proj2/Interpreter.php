<?php

namespace IPP\Student;

use IPP\Core\AbstractInterpreter;

class Interpreter extends AbstractInterpreter
{
    public function execute(): int
    {
        $dom = $this->source->getDOMDocument();

        $executor = new Executor($dom, $this->input, $this->stdout);
        $executor->execute();

        return 0;
    }
}
