/* htab_lookup_add.c
 * Řešení IJC-DU2, příklad 2), 25.04.2024
 * Autor: Radim Dvořák, xdvorar00, FIT
 * Přeloženo: gcc 11.4.0
 */

#include <stdlib.h>
#include <stdio.h>
#include "htab_struct_private.h"

//Vymazání určitého záznamu
bool htab_erase(htab_t * t, htab_key_t key){
    //Najití indexu v arr_ptr
    size_t index = htab_hash_function(key) % htab_bucket_count(t);
    struct htab_item *current = t->arr_ptr[index];

    struct htab_item *tmp = NULL;
    //Projetí všech záznamů a srovnání s klíčem
    while (current != NULL) {
        if (strcmp(current->pair->key, key) == 0) {
            //Je poslední v seznamu
            if(tmp == NULL){
                 t->arr_ptr[index] = current->next;
            }
            else{
                tmp->next = current->next;
            }
            free((void *)current->pair->key);
            free(current->pair);
            free(current);

            t->size--;
            return true;
        }
        tmp = current;
        current = current->next;
    }
    return false;
}