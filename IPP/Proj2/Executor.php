<?php

namespace IPP\Student;

use IPP\Student\Object\BaseObject;
use IPP\Student\Object\IntegerObject;
use IPP\Student\Object\ClassObject;
use IPP\Student\Object\TrueObject;
use IPP\Student\Object\FalseObject;
use IPP\Student\Object\NilObject;
use IPP\Student\Object\StringObject;
use IPP\Student\Stack\Stack;
use IPP\Student\Stack\StackFrame;
use IPP\Core\Interface\InputReader;
use IPP\Core\Interface\OutputWriter;
use IPP\Student\Structure\ExprStructure;
use IPP\Student\Structure\ProgramStructure;
use IPP\Student\Structure\MethodStructure;
use IPP\Student\Structure\SendStructure;
use IPP\Student\Structure\LiteralStructure;
use IPP\Student\Structure\BlockStructure;

class Executor
{
    private ProgramStructure $program;
    private Stack $stack;
    private InputReader $input;
    private OutputWriter $output;

    public function __construct(\DOMDocument $program, InputReader $input, OutputWriter $output)
    {
        $this->program = new ProgramStructure($program);
        $this->stack = new Stack();
        $this->input = $input;
        $this->output = $output;
    }

    public function execute()
    {
        $mainClass = $this->program->getClass('Main');
        if ($mainClass != null && $mainClass->hasMethod('run')) {
            $run = $mainClass->getMethod('run');
            $this->invokeMethod($run);
        } else {
            throw new CustomExceptions(31);
        }
    }

    private function invokeMethod(MethodStructure $method): BaseObject
    {
        $result = null;
        foreach ($method->assigns as $assign) {
            $receiver = $assign->expr;
            $resultName = $assign->result;

            if ($receiver->base instanceof SendStructure) {
                $result = $this->resolveSend($receiver->base);
                $this->stack->appendToStackFrame($resultName, $result);
            } elseif ($receiver->base instanceof ExprStructure) {
                $result = $this->resolveExpression($receiver->base);
            } elseif ($receiver->base instanceof LiteralStructure) {
                $result = $this->fromObject($receiver->base->class, $receiver->base->value);
                $this->stack->appendToStackFrame($resultName, $result);
            } elseif ($receiver->base instanceof BlockStructure) {
                $result = $this->resolveBlock($receiver->base);
                $this->stack->appendToStackFrame($resultName, $result);
            } else {
                $result = $this->stack->findObject($receiver->base);
                if ($result instanceof BaseObject) {
                    $this->stack->appendToStackFrame($resultName, $result);
                }
            }
        }
        return $result;
    }

    /**
     * @param string $methodName
     * @param ExprStructure[] $arguments
     * @param ExprStructure $receiver
     */
    public function invokeMethodByName(string $methodName, array $arguments, ExprStructure $receiver): ?BaseObject
    {

        $method = null;
        foreach ($this->program->classes as $class) {
            if ($class->hasMethod($methodName)) {
                $method = $class->getMethod($methodName);
                $this->stack->push(new StackFrame($methodName, $this->handleArguments($arguments), []));
                $this->invokeMethod($method);
                $this->stack->pop();
            }
        }
        try {
            $receiverObject = $this->resolveReceiver($receiver);

            $methodName = str_replace(":", "", $methodName);

            if ($methodName == "ifTrueifFalse") {
                $resultFunction = $receiverObject->{$methodName}();
                if ($resultFunction instanceof TrueObject) {
                    $result = $this->resolveExpression($arguments[0]);
                } else {
                    $result = $this->resolveExpression($arguments[1]);
                }
            } else {
                $result = $receiverObject->{$methodName}(...($this->handleArguments($arguments)));
            }
        } catch (\Throwable $e) {
            throw new CustomExceptions(51);
        }

        return $result;
    }

    private function handleArguments(array $arguments): array
    {
        $result = [];
        foreach ($arguments as $arg) {
            if ($arg instanceof ExprStructure) {
                $result[] = $this->resolveExpression($arg);
                //
            } elseif ($arg instanceof LiteralStructure) {
                $result[] = $this->fromObject($arg->class, $arg->value);
            } elseif ($arg instanceof SendStructure) {
                $result[] = $this->resolveSend($arg);
            } elseif ($arg instanceof BlockStructure) {
                $result[] = $this->resolveBlock($arg);
            }
        }
        return $result;
    }

    private function resolveExpression(ExprStructure $expr): BaseObject
    {
        if ($expr->base instanceof SendStructure) {
            return $this->resolveSend($expr->base);
        } elseif ($expr->base instanceof ExprStructure) {
            return $this->resolveExpression($expr->base);
        } elseif ($expr->base instanceof LiteralStructure) {
            return $this->fromObject($expr->base->class, $expr->base->value);
        } elseif ($expr->base instanceof BlockStructure) {
            return $this->resolveBlock($expr->base);
        } else {
            return $this->stack->findObject($expr->base);
        }
    }

    private function resolveReceiver(ExprStructure $expr): BaseObject
    {
        return $this->resolveExpression($expr);
    }

    private function resolveSend(SendStructure $expr): BaseObject
    {
        $selector = $expr;
        if ($selector->selector == "new") {
            $result = $this->newObject($selector->getReceiverType());
            return $result;
        } elseif ($selector->selector == "from:") {
            $arguments = $this->handleArguments($selector->getArguments());
            if (sizeof($arguments) != 1) {
                throw new CustomExceptions(31);
            } else {
                $result = $this->fromObject($selector->getReceiverType(), $arguments[0]);
            }
            return $result;
        } else {
            $result = $this->invokeMethodByName($selector->selector, $selector->getArguments(), $expr->receiver);
            return $result;
        }
    }

    private function resolveBlock(BlockStructure $block): BaseObject
    {
        $this->stack->push(new StackFrame("Block", [], []));
        $assigns = $block->getBlock();
        $result = null;

        foreach ($assigns as $assign) {
            $receiver = $assign->expr;
            $resultName = $assign->result;

            if ($receiver->base instanceof SendStructure) {
                $result = $this->resolveSend($receiver->base);
                $this->stack->appendToStackFrame($resultName, $result);
            } elseif ($receiver->base instanceof ExprStructure) {
                $result = $this->resolveExpression($receiver->base);
                $this->stack->appendToStackFrame($resultName, $result);
            } elseif ($receiver->base instanceof LiteralStructure) {
                $result = $this->fromObject($receiver->base->class, $receiver->base->value);
                $this->stack->appendToStackFrame($resultName, $result);
            } elseif ($receiver->base instanceof BlockStructure) {
                $result = $this->resolveBlock($receiver->base);
                $this->stack->appendToStackFrame($resultName, $result);
            }
        }
        $this->stack->pop();
        return $result;
    }

    private function newObject(string $name): BaseObject
    {
        if ($name == "True") {
            $result = TrueObject::getInstance();
        } elseif ($name == "False") {
            $result = FalseObject::getInstance();
        } elseif ($name == "Nil") {
            $result = NilObject::getInstance();
        } elseif ($name == "Object") {
            $result = new BaseObject($this->input, $this->output);
        } elseif ($name == "Integer") {
            $result = new IntegerObject($this->input, $this->output, 0);
        } elseif ($name == "String") {
            $result = new StringObject($this->input, $this->output, "");
        } else {
            try {
                $className = "IPP\\Student\\Object\\" . $name . "Object";
                $result = new $className($this->input, $this->output, "");
            } catch (\Throwable $e) {
                $result = new ClassObject($this->input, $this->output, $name);
            }
        }
        return $result;
    }

    private function fromObject(string $name, $value = null): BaseObject
    {
        //
        if ($name == "True") {
            $result = TrueObject::getInstance();
        } elseif ($name == "False") {
            $result = FalseObject::getInstance();
        } elseif ($name == "Nil") {
            $result = NilObject::getInstance();
        } elseif ($name == "Object") {
            $result = new BaseObject($this->input, $this->output, $value);
        } elseif ($name == "Integer") {
            if ($value == null) {
                $result = new IntegerObject($this->input, $this->output, 0);
            } else {
                if ($value instanceof IntegerObject) {
                    return $value;
                } else {
                    $result = new IntegerObject($this->input, $this->output, $value);
                }
            }
        } elseif ($name == "String") {
            if ($value == null) {
                $result = new StringObject($this->input, $this->output, "");
            } else {
                if ($value instanceof StringObject) {
                    return $value;
                } else {
                    $result = new StringObject($this->input, $this->output, $value);
                }
            }
        } else {
            $result = new ClassObject($this->input, $this->output, $name);
        }
        return $result;
    }
}
