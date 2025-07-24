// Projekt: Implementace překladače imperativního jazyka IFJ24
// Autor: Marek Slaný   (xslany03)

#ifndef SCOPE_STACK_H
#define SCOPE_STACK_H

#include "vector.h"
#include <stddef.h>
#include <stdint.h>

// Stack of scope numbers, used to get unique scope id for each scope
// inside a function
typedef struct {
    vector* stack;
} scope_stack;

scope_stack* scope_stack_init(size_t init_capacity);
void scope_stack_free(scope_stack** scp_s);
uint16_t* push_scope(scope_stack* scp_s);
void pop_scope(scope_stack* scp_s);
char* get_scope_id(scope_stack* scp_s);

#endif // SCOPE_STACK_H
