/* bitset.h
 * Řešení IJC-DU1, příklad a), 26.03.2024
 * Autor: Radim Dvořák, xdvorar00, FIT
 * Přeloženo: gcc 11.4.0
 */

#include <stdlib.h>
#include <string.h>
#include "bitset.h"


#ifdef USE_INLINE

extern inline void bitset_free(bitset_t jmeno_pole);

extern inline bitset_index_t bitset_size(bitset_t jmeno_pole);

extern inline void bitset_fill(bitset_t jmeno_pole, bool bool_výraz);

extern inline void bitset_setbit(bitset_t jmeno_pole,bitset_index_t index,bool bool_výraz);

extern inline bool bitset_getbit(bitset_t jmeno_pole,bitset_index_t index);

#endif