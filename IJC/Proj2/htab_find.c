/* htab_find.c
 * Řešení IJC-DU2, příklad 2), 25.04.2024
 * Autor: Radim Dvořák, xdvorar00, FIT
 * Přeloženo: gcc 11.4.0
 */

#include <stdlib.h>
#include "htab_struct_private.h"

htab_pair_t * htab_find(const htab_t * t, htab_key_t key){
    //Najití indexu v arr_ptr
    size_t index = htab_hash_function(key) % htab_bucket_count(t);
    struct htab_item *item = t->arr_ptr[index];

    //Projetí všech záznamů a srovnání s klíčem
    while (item != NULL)
    {
        if (strcmp(item->pair->key, key) == 0) return item->pair;
        item = item->next;
    }
    return NULL;
}