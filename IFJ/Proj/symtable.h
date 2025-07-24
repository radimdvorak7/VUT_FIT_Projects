// Projekt: Implementace překladače imperativního jazyka IFJ24
// Autor: Dominik Václavík (xvacla37), Marek Slaný (xslany03)

#ifndef IFJ_PROJEKT_2024_SYMTABLE_H
#define IFJ_PROJEKT_2024_SYMTABLE_H

#include "data_types.h"
#include "vector.h"
#include <stdint.h>

#define INSERT_SUCCESS 0
#define INSERT_ALREADY_IN 1
#define INSERT_MEM_ERR 2

typedef enum { BLACK, RED } t_color;

// What the declared symbol is.
typedef enum { SYM_VAR, SYM_CONST, SYM_FUNC} sym_type;

// Stores information about the symbol parameters
typedef struct {
    char* param_name;
    data_type param_type;
} sym_param;

// Data associated with the symbol.
// TODO: optimization: Store comptime const values inside and then replace the
// const variables with their values in AST.
typedef struct {
    data_type return_type; // Symbol's declared data type
    sym_type symbol_type;
    bool initialized; // Whether the symbol already has a value asigned
    bool used;
    bool muted;
    union {
        vector* func_parameters; // 'vector' of 'sym_param'
        char* scope_id;
    };
} b_tree_data;

typedef struct rb_tree_node {
    struct rb_tree_node* parent;
    struct rb_tree_node* left;
    struct rb_tree_node* right;
    t_color color;
    char* symbol;
    b_tree_data data;
} b_node;

typedef struct {
    b_node* root;
} b_tree;

void rb_tree_init(b_tree* tree);

int rb_tree_insert(b_tree* tree, char* symbol, b_tree_data data);
b_tree_data* rb_tree_find(b_tree* tree, const char* symbol);
void rb_tree_free(b_tree* tree);
void rb_tree_print(b_tree* tree);

#endif
