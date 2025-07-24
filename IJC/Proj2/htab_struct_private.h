/* htab_struct_private.h
 * Řešení IJC-DU2, příklad 2), 25.04.2024
 * Autor: Radim Dvořák, xdvorar00, FIT
 * Přeloženo: gcc 11.4.0
 */
#ifndef HTAB_PRIV_H
#define HTAB_PRIV_H
#include "htab.h"

struct htab_item {
    htab_pair_t *pair;
    struct htab_item *next;
};

struct htab {
    size_t size;
    size_t arr_size;
    struct htab_item *arr_ptr[];
};

#endif