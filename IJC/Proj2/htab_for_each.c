/* htab_for_each.c
 * Řešení IJC-DU2, příklad 2), 25.04.2024
 * Autor: Radim Dvořák, xdvorar00, FIT
 * Přeloženo: gcc 11.4.0
 */

#include <stdlib.h>
#include "htab_struct_private.h"

//Provede funkci nad každým záznamem
void htab_for_each(const htab_t * t, void (*f)(htab_pair_t *data)){
    for(size_t i = 0; i < t->arr_size; i++){
        struct htab_item *item = t->arr_ptr[i];
        while(item != NULL){
            htab_pair_t pair = {item->pair->key, item->pair->value};
            f(&pair);
            item = item->next;
        }
    }
}