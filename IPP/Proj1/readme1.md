## Implementační dokumentace k 1. úloze do IPP 2024/2025
## Jméno a příjmení: Radim Dvořák
## Login: xdvorar00

# Implementation details
## Lexcial and syntatical analysis
Both parts are done at the same time, using parser from the library lark. 
All non-terminals are descriped as regural expressions.

Grammar was slightly modified from one given in assigment, there were added some terminals to better suit the generation.
Checking if in variable are keywords is done later in semantics part, where the already existing tree is searched for keywords in position of variable and selector without parameters.

The only flaw in these parts is accepting incorrect string literals, especially it accepts escape sequence '\x'.

## Semantics
This part was implemented only partially. It was done using Transformer from lark library on existing lark tree, where semantic informations were added as nodes to the tree.
The errors that unfortunately i am not checking are:
    Undefined class
    Undefined variables in blocks
    Undefined class methods
    Collision and redefiniton of variables
    Method redefiniton

## Generation
The syntax tree was transformed into XML tree via calling function toXML, which when given node determined if it was inner node, it got called on all subtrees, or leaf, there it was changed directly into element of ElementTree.

For node 'name' its atrributes were in nodes 'name_.*' where part after undescore is attribute name and the only child of this node is the attribute value.

More complex structures like nested expression, selector with multiple parameters and using blocks in expressions are being generated wrong.