// Projekt: Implementace překladače imperativního jazyka IFJ24
// Autor: Marek Slaný   (xslany03)

#include "scope_stack.h"
#include "vector.h"
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

// Initializes the 'scope_stack' with stack of 'init_capacity'
scope_stack* scope_stack_init(size_t init_capacity) {
    scope_stack* scp_s = malloc(sizeof(scope_stack));
    if (scp_s == NULL) {
        return NULL;
    }
    scp_s->stack = vec_init(init_capacity, sizeof(uint16_t));
    // Failed to init the inner stack
    if (scp_s->stack == NULL) {
        free(scp_s);
        return NULL;
    }

    uint16_t zero = 0;
    if(vec_push(scp_s->stack, &zero) == NULL){
        free(scp_s);
        return NULL;
    }

    return scp_s;
}

// Frees the 'scope_stack' and sets the associated pointer to NULL.
void scope_stack_free(scope_stack** scp_s) {
    vec_free(&(*scp_s)->stack);
    free(*scp_s);
    *scp_s = NULL;
}

// Pushes new scope number onto the stack
uint16_t* push_scope(scope_stack* scp_s) {
    uint16_t zero = 0;
    uint16_t* ptr = vec_push(scp_s->stack, &zero);
    if(ptr == NULL){
        return NULL;
    }

    (*(ptr - 1))++;

    return ptr;
}

// Pops the topmost scope id
void pop_scope(scope_stack* scp_s) {
    vec_remove_last(scp_s->stack);
}

// Concatenates all the ids together with $ in between to construct unique
// scope prefix. Returns the prefix as char* or NULL on failure. The caller
// is responsible for freeing the returned prefix.
char* get_scope_id(scope_stack* scp_s) {
    vector* dyn_str = vec_init(4, sizeof(char));
    if (dyn_str == NULL) {
        return NULL;
    }
    // To store the string of the 'uint16_t'
    char buf[6];
    char* c;

    for (size_t i = 0; i < scp_s->stack->len - 1; i++) {
        uint16_t* id = vec_get(scp_s->stack, i);

        if (id == NULL) {
            vec_free(&dyn_str);
            return NULL;
        }

        c = vec_pushchar(dyn_str, '$');
        if (c == NULL) {
            vec_free(&dyn_str);
            return NULL;
        }

        int digits = snprintf(NULL, 0, "%u", *id);
        int ret = snprintf(buf, digits + 1, "%u", *id);

        // Could not write all the digits to the string
        if (digits + 1 <= ret || ret < 0) {
            vec_free(&dyn_str);
            return NULL;
        }

        for (int j = 0; j < digits; j++) {
            c = vec_pushchar(dyn_str, buf[j]);
            if (c == NULL) {
                vec_free(&dyn_str);
                return NULL;
            }
        }
    }

    return vec_to_str(&dyn_str);
}
