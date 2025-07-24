// Projekt: Implementace přeladače jazyka IFJ24
// Autor: Dominik Václavík (xvacla37)

#include "code_gen.h"
#include "error.h"
#include "parser.h"
#include "scope_stack.h"
#include "semantic.h"
#include "symtable.h"
#include "vector.h"
#include <stdio.h>

int main() {

    DEBUG_PRINT("%s", "NDEBUG not defined, debugging.")

    node* ast_tree = create_node(PROG);
    if (ast_tree == NULL) {
        fprintf(stderr, "Failed to alocate memory!\n Ending program!\n");
        return INTERNAL_ERR;
    }

    error_code err = parse(ast_tree);
    DEBUG_PRINT("parsing ended with code %d", err)

    if (err != NO_ERR) {
        fprintf(stderr, "The program encountered a problem during semantic or "
                        "lexical analysis. Aborting...\n");
        delete_tree(&ast_tree);
        return err;
    }

    vector* symtable_stack = vec_init(4, sizeof(b_tree*));

    if (symtable_stack == NULL) {
        fprintf(stderr, "Failed to alocate memory!\n Ending program!\n");
        return INTERNAL_ERR;
    }

    scope_stack* scp_s = scope_stack_init(3);
    if (scp_s == NULL) {
        fprintf(stderr, "Failed to alocate memory!\n Ending program!\n");
        return INTERNAL_ERR;
    }

    err = enter_scope(symtable_stack);
    if (err != NO_ERR) {
        return err;
    }
    int insert_status = populate_with_builtins(symtable_stack);
    if (insert_status != INSERT_SUCCESS) {
        return INTERNAL_ERR;
    }

    DEBUG_PRINT("entering PROG scope with %s", ERROR_STRINGS[err]);

    err = semantically_analyse(ast_tree, symtable_stack, scp_s);
    if (err != NO_ERR) {
        fprintf(stderr, "The program encountered a problem during semantical "
                        "analysis. Aborting...\n");
        return err;
    }

    DEBUG_PRINT("semantical analysis ended with %s", ERROR_STRINGS[err]);

    leave_scope(symtable_stack);

    scope_stack_free(&scp_s);
    vec_free(&symtable_stack);

    err = generate_code(ast_tree);
    if (err != NO_ERR) {
        fprintf(stderr, "The program encountered a problem during code generation "
                        ". Aborting...\n");
        return err;
    }

    delete_tree(&ast_tree); // TODO: free allocated strings from some tree nodes
                            // by TYPE. (IDENTIFIER, FUNCTION)
    return NO_ERR;
}
