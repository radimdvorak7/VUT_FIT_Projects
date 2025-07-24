<?php

namespace IPP\Student\Structure;

use DOMDocument;
use DOMXPath;

class ProgramStructure
{
    /**
     * @var ClassStructure[] An array of ClassStructures
     */
    public array $classes = [];

    public function __construct(\DOMDocument $program)
    {
        $xpath = new \DOMXPath($program);
        $start_program = $xpath->query('//class');

        foreach ($start_program as $class) {
            if ($class instanceof \DOMElement) {
                $classStructure = new ClassStructure($class);
                $this->addClass($classStructure);
            }
        }
    }

    public function addClass(ClassStructure $class): void
    {
        $this->classes[$class->name] = $class;
    }

    public function getClass(string $name): ?ClassStructure
    {
        return $this->classes[$name] ?? null;
    }
}
