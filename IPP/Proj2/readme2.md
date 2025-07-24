# Documentation of Project Implementation for IPP 2024/2025
Name and surname: Radim Dvořák
Login: xdvorar00

## Table of contents

1. [Table of contents](#table-of-contents)
2. [Project summary](#project-summary)
3. [Design of interpret](#design-of-interpret)
    - [Not fully implemented features](#not-fully-implemented-features)

## Project summary

This project is interpret for object-oriented programming language SOL25. Was written in PHP and is run on PHP 8.4.\


## Design of interpret

Interpret was designed, that all builtin classes will have corresponding class in PHP. All other classes should be read directly from XML file. 

First step is to convert XML into `Structure` classes, which helps interpreting the code.
This is done by using the `XMLParser` class, which parses the XML file and creates a tree of `Structure` objects. Each `Structure` object represents a class, method, or attribute in the input XML file. 

Inheritance was designed, that all would be iteratively searched by the `parent` attribute in the in their tag `class`. If the parent class is not found, the interpreter will continue searching in the for the built-in classes, that are declared in folder `Object`. If the parent class is not found in the built-in classes, the interpreter will throw an error.


Objects of user-defined, defined in XML, are instantiated by the `ClassObject` class. All objects are uniquely by their class name and the name of the object. 

### Not fully implemented features

- Reading the input
- Complete checking of correctness of the XML tree
- Special keywords *self* and *super*, this means that inner attributes of classes are not implemented
- Inheritance of methods from parent class
- Blocks as objects
- Some builtin methods ( `timesRepeat:`)
