// Projekt: Implementace překladače imperativního jazyka IFJ24
// Autor: Marek Slaný   (xslany03)
//
// Implementation of semantic analysis

#ifndef SEMANTIC_H
#define SEMANTIC_H

#include "ast.h"
#include "error.h"
#include "scope_stack.h"
#include "vector.h"

error_code semantically_analyse(node* ast, vector* symtable_stack,
                                scope_stack* scp_s);
error_code enter_scope(vector* symtable_stack);
error_code leave_scope(vector* symtable_stack);
int populate_with_builtins(vector* symtable_stack);

#endif // SEMANTIC_H
