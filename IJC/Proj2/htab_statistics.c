/* htab_statistics.c
 * Řešení IJC-DU2, příklad 2), 25.04.2024
 * Autor: Radim Dvořák, xdvorar00, FIT
 * Přeloženo: gcc 11.4.0
 */

#include <stdio.h> 
#include <limits.h>
#include <stdint.h>
#include "htab_struct_private.h"

//Vypsání statistik o tabulce
void htab_statistics(const htab_t * t){
    size_t min = SIZE_MAX;
    size_t max = 0;
    
    //Projetí všech seznamů v tabulce
    for(size_t i = 0; i < t->arr_size; i++){
        size_t count = 0;
        struct htab_item *item = t->arr_ptr[i];
        while (item != NULL)
        {   
            count++;
            item = item->next;
        }
        if(count < min){
            min = count;
        }
        if (count > max) {
            max = count;
        }
    }
    
    fprintf(stderr, "Average: %f\n", (double)t->size / (double)t->arr_size);
    fprintf(stderr, "Min: %f\n", (double)min);
    fprintf(stderr, "Max: %f\n", (double)max);
}