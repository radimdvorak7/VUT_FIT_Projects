// Projekt: Implementace přeladače jazyka IFJ24
// Autor: Dominik Václavík (xvacla37)
#ifndef IFJ_PROJEKT_2024_CODE_GEN_HELP_H
#define IFJ_PROJEKT_2024_CODE_GEN_HELP_H

#include <stdio.h>
#include <stdbool.h>

typedef struct label_data {
    char *name;
    char* scope;
} l_data;


typedef struct label_stack {
    l_data* stack;
    size_t max;
    size_t current;
} l_stack;

void print_string_format(FILE* fptr, const char* string);
l_stack* l_stack_init();
bool l_stack_push(l_stack* stack, l_data* label);
void l_stack_pop(l_stack* stack, l_data* label);
void l_stack_free(l_stack** stack);

#endif