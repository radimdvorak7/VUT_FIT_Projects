/* error.c
 * Řešení IJC-DU1, příklad b), 26.03.2024
 * Autor: Radim Dvořák, xdvorar00, FIT
 * Přeloženo: gcc 11.4.0
 */

#include <stdarg.h>
#include <stdio.h>
#include "error.h"

//Vypise chybove hlaseni
void warning(const char *fmt, ... ){
    va_list args;
    va_start(args, fmt);
    fprintf(stderr, "Warning: ");
    vfprintf(stderr, fmt, args);
    va_end(args);
}
//Vypise chybove hlaseni a vypne program
void error_exit(const char *fmt, ... ){
    va_list args;
    va_start(args, fmt);
    fprintf(stderr, "Error: ");
    vfprintf(stderr, fmt, args);
    va_end(args);
    exit(EXIT_FAILURE);
}