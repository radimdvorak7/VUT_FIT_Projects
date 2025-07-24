/* htab_clear.c
 * Řešení IJC-DU2, příklad 2), 25.04.2024
 * Autor: Radim Dvořák, xdvorar00, FIT
 * Přeloženo: gcc 11.4.0
 */

#include <stdlib.h>
#include "htab_struct_private.h"

//Vymazání všech záznamů
void htab_clear(htab_t * t){
    //Projetí všech položek arr_ptr
    for (size_t i = 0; i < t->arr_size; i++) {
        struct htab_item *current = t->arr_ptr[i];
        //Projetí všech položek na arr_ptr[i]
        while (current  != NULL) {
            free((void *)current->pair->key);
            free(current ->pair);
            struct htab_item *tmp = current;
            current = current->next;
            free(tmp);
        }
    }
    t->size=0;
}