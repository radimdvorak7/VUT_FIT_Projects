// Projekt: Implementace přeladače jazyka IFJ24
// Autor: Dominik Václavík (xvacla37)
#ifndef IFJ_PROJEKT_2024_FUNC_LOOK_UP_H
#define IFJ_PROJEKT_2024_FUNC_LOOK_UP_H

#include <stdio.h>

#include "error.h"

#define INSERT_SUCCESS 0
#define INSERT_ALREADY_IN 1
#define INSERT_MEM_ERR 2

typedef enum { BLACK, RED } t_color;

typedef struct fl_tree_data {
    char* translated;
    char* code;
    int counter;
} fl_data;

typedef struct fl_tree_node {
    struct fl_tree_node* parent;
    struct fl_tree_node* left;
    struct fl_tree_node* right;
    t_color color;
    char* symbol;
    fl_data data;
} fl_node;

typedef struct {
    fl_node* root;
} fl_tree;

void fl_tree_init(fl_tree* tree);

error_code fl_tree_insert_functions(fl_tree* tree);
char* fl_tree_use(fl_tree* tree, const char* func_id);
void fl_tree_include(fl_tree* tree, FILE* fptr);

void fl_tree_free(fl_tree* tree);

#endif