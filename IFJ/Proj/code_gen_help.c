// Projekt: Implementace přeladače jazyka IFJ24
// Autor: Dominik Václavík (xvacla37)

#include "code_gen_help.h"

#include <stdlib.h>

void print_string_format(FILE* fptr, const char* string){
    size_t offset = 0;
    char c;
    while((c = string[offset]) != '\0'){
        if((0 <= c && c <= 32) || c == 35 || c == 92){
            fprintf(fptr, "\\%03d", c);
        }
        else{
            fputc(c, fptr);
        }
        offset++;
    }
}

#define L_STACK_ALLOC_BLOCK_SIZE 8
l_stack* l_stack_init(){
    l_stack* ptr = malloc(sizeof(l_stack));
    if(ptr == NULL){
        return NULL;
    }
    ptr->stack = malloc(sizeof(l_data) * L_STACK_ALLOC_BLOCK_SIZE);
    if(ptr->stack == NULL){
        free(ptr);
        return NULL;
    }
    ptr->current = 0;
    ptr->max = L_STACK_ALLOC_BLOCK_SIZE;

    return ptr;
}

bool l_stack_push(l_stack* stack, l_data* label){
    if(stack->current >= stack->max){
        l_data* newPtr = realloc(stack->stack, sizeof(l_data) * (stack->max * 2));
        if(newPtr == NULL){
            return false;
        }
        stack->stack = newPtr;
        stack->max *= 2;
    }

    stack->stack[stack->current] = *label;
    stack->current++;

    return true;
}

void l_stack_pop(l_stack* stack, l_data* label){
    stack->current--;
    if(stack->current == 0){
        l_data d = {0};
        *label = d;
    }
    else {
        *label = stack->stack[stack->current - 1];
    }
}

void l_stack_free(l_stack** stack){
    free((*stack)->stack);
    free(*stack);
    *stack = NULL;
}