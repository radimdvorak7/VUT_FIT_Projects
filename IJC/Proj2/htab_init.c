/* htab_init.c
 * Řešení IJC-DU2, příklad 2), 25.04.2024
 * Autor: Radim Dvořák, xdvorar00, FIT
 * Přeloženo: gcc 11.4.0
 */

#include <stdlib.h>
#include "htab_struct_private.h"

//Vytvoření hashovací tabulky
htab_t *htab_init(const size_t n){
    htab_t *hash_table = (htab_t *) malloc(sizeof(struct htab) + (n * sizeof(struct htab_item)));
    if(hash_table == NULL) return NULL;

    hash_table->size=0;
    hash_table->arr_size= n;
    for(size_t i = 0; i < hash_table->arr_size;i++){
        hash_table->arr_ptr[i] = NULL;
    }
    
    return hash_table;
}