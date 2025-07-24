/* htab_size.c
 * Řešení IJC-DU2, příklad 2), 25.04.2024
 * Autor: Radim Dvořák, xdvorar00, FIT
 * Přeloženo: gcc 11.4.0
 */

#include "htab_struct_private.h"

//Počet prvků tabulky
size_t htab_size(const htab_t * t){
    return t->size;
}