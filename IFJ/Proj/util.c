// Projekt: Implementace překladače imperativního jazyka IFJ24
// Autoři: Dominik Václavík (xvacla37), Marek Slaný (xslany03)

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <stdbool.h>

// Duplicates the string, returns NULL if it fails.
char* d_string(const char* str) {
    assert(str != NULL);
    unsigned long mem_size = sizeof(char) * (strlen(str) + 1);
    char* ptr = malloc(mem_size);
    if (ptr == NULL) {
        return NULL;
    }
    memcpy(ptr, str, mem_size);
    return ptr;
}

// Returns true if 'd' has decimal places
bool has_decimals(double d) {
    return (d - (int)d) != 0;
}
