/* error.h
 * Řešení IJC-DU1, příklad b), 26.03.2024
 * Autor: Radim Dvořák, xdvorar00, FIT
 * Přeloženo: gcc 11.4.0
 */

#ifndef ERROR_H
#define ERROR_H

#include <stdlib.h>

void warning(const char *fmt, ...);

void error_exit(const char *fmt, ...);

#endif