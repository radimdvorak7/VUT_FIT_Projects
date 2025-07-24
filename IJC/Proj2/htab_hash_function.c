/* htab_hash_function.c
 * Řešení IJC-DU2, příklad 2), 25.04.2024
 * Autor: Radim Dvořák, xdvorar00, FIT
 * Přeloženo: gcc 11.4.0
 */

#include <stdlib.h>
#include <stdint.h>
#include "htab_struct_private.h"

//Hashovací funkce
size_t htab_hash_function(htab_key_t str) {
    uint32_t h=0;
    htab_key_t p;
    for(p=str; *p!='\0'; p++){
        h = 65599*h + *p;
    }
    return h;
}