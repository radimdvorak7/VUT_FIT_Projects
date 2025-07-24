<?php

namespace IPP\Student\Structure;

use Exception;
use IPP\Student\Object\BaseObject;

class SendStructure
{
    public string $selector;
    /**
     * @var ExprStructure[]
     */
    public array $parameters = [];

    public ExprStructure $receiver;

    public function __construct(\DOMElement $send)
    {
        $this->selector = $send->getAttribute('selector');

        foreach ($send->childNodes as $parameter) {
            if ($parameter instanceof \DOMElement) {
                if ($parameter->nodeName === 'expr') {
                    //
                    $this->receiver = new ExprStructure($parameter);
                } elseif ($parameter->nodeName === 'arg') {
                    //
                    $order = $parameter->getAttribute("order");
                    $this->parameters["{$order}"] = new ExprStructure($parameter);
                }
            }
        }
    }

    /**
     * @return ExprStructure[]
     */
    public function getArguments(): array
    {
        if (!empty($this->parameters)) {
            $args = [];
            foreach ($this->parameters as $parameter) {
                //
                $args[] = $parameter;
            }
            return $args;
        }
        return [];
    }


    public function getReceiverType(): string
    {
        if ($this->receiver->base instanceof LiteralStructure) {
            return $this->receiver->baseType;
        } elseif ($this->receiver->base instanceof ExprStructure) {
            return $this->receiver->base->getReceiverType();
        } elseif ($this->receiver->base instanceof SendStructure) {
            return $this->receiver->base->getReceiverType();
        } else {
            return $this->receiver->baseType;
        }
    }
}
