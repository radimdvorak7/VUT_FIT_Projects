/* htab_lookup_add.c
 * Řešení IJC-DU2, příklad 2), 25.04.2024
 * Autor: Radim Dvořák, xdvorar00, FIT
 * Přeloženo: gcc 11.4.0
 */

#include <stdlib.h>
#include <stdio.h>
#include "htab_struct_private.h"

char *copy_string(const char *str) {
    int lenght = strlen(str) + 1;
    char *dst = malloc(lenght);
    if (dst == NULL) return NULL;        
    strncpy(dst, str,lenght);                   
    return dst;    
}

htab_pair_t * htab_lookup_add(htab_t * t, htab_key_t key){  
    //Najití indexu v arr_ptr
    size_t index = htab_hash_function(key) % htab_bucket_count(t);
    struct htab_item *current = t->arr_ptr[index];

    //Projetí všech záznamů a srovnání s klíčem
    while (current != NULL) {
        if (strcmp(current->pair->key, key) == 0) {
            current->pair->value++;
            return current->pair;
        }
        current = current->next;
    }

    //Alokování záznamu
    struct htab_item *new_item = malloc(sizeof(struct htab_item));
    if (new_item == NULL) {
        return NULL;
    }
    new_item->next = NULL;
    
    //Alokování páru
    new_item->pair = malloc(sizeof(htab_pair_t));
    if (new_item->pair == NULL) {
        free(new_item);
        return NULL;
    }
    //Alokování klíče
    new_item->pair->key = copy_string(key);
    if(new_item->pair->key==NULL){
        free(new_item->pair);
        free(new_item);
        return NULL;
    }
    //První v seznamu
    if(t->arr_ptr[index] == NULL){
        t->arr_ptr[index] = new_item;
        new_item->pair->value = 1;
        t->size++;
        return new_item->pair;
    }
    //Najití posledního záznamu v seznamu
    struct htab_item *tmp = t->arr_ptr[index];
    while (tmp->next != NULL) {
        tmp = tmp->next;
    }

    new_item->pair->value = 1;
    tmp->next = new_item;

    t->size++;
    return new_item->pair;
}