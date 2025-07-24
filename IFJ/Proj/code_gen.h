// Projekt: Implementace přeladače jazyka IFJ24
// Autor: Dominik Václavík (xvacla37)
#ifndef IFJ_PROJEKT_2024_CODE_GEN_H
#define IFJ_PROJEKT_2024_CODE_GEN_H

#include "ast.h"
#include "error.h"

error_code generate_code(node* ast_tree);

#endif
