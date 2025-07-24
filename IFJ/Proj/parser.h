// Project: Implementation of a compiler for the IFJ24 imperative language.
// Author: František Sedláček ( xsedla2d )
//
// Main file of Syntactic parser, where recursive descent is utilized with LL rules.

#ifndef IFJ_PROJEKT_2024_PARSER_H
#define IFJ_PROJEKT_2024_PARSER_H

#include "ast.h"
#include "error.h"

error_code parse(node* synroot);

#endif // IFJ_PROJEKT_2024_PARSER_H
