/* htab_free.c
 * Řešení IJC-DU2, příklad 2), 25.04.2024
 * Autor: Radim Dvořák, xdvorar00, FIT
 * Přeloženo: gcc 11.4.0
 */

#include <stdlib.h>
#include "htab_struct_private.h"

//Uvolnění tabulky
void htab_free(htab_t *t){
    htab_clear(t);
    free(t);
}