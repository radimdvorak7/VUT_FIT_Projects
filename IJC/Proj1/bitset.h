/* bitset.h
 * Řešení IJC-DU1, příklad a), 26.03.2024
 * Autor: Radim Dvořák, xdvorar00, FIT
 * Přeloženo: gcc 11.4.0
 */

#ifndef BITSET_H
#define BITSET_H

#include <stdbool.h>
#include <assert.h>
#include <stdlib.h>
#include <limits.h>
#include <string.h>
#include "error.h"

typedef unsigned long bitset_t[];
typedef unsigned long bitset_index_t;

#define LONG_SIZE (sizeof(unsigned long))
#define LONG_BIT (LONG_SIZE * CHAR_BIT)

#define ARRAY_SIZE(velikost) ((((velikost)/LONG_BIT) + ((((velikost) % LONG_BIT) + 1) != 0) + 1))
#define ARRAY_INDEX(index) (((index)/LONG_BIT) + ((((index) % LONG_BIT) + 1) != 0))

#define BIT_MASK(index) (1UL << ((index)%(LONG_BIT)))

//Vytvari bitove pole dane velikosti a inicializuje ho nulami, na 1. pozici pole zapise velikost bitoveho pole
#define bitset_create(jmeno_pole,velikost)\
    unsigned long jmeno_pole[ARRAY_SIZE(velikost)] = {(velikost),0};\
    static_assert( (velikost) > 0 , "bitset_create: Chyba překladu\n")
    
//Dynamicky alokuje bitove pole dane velikosti a inicializuje ho nulami, na 1. pozici pole zapise velikost bitoveho pole
#define bitset_alloc(jmeno_pole,velikost)\
    assert( ((void)"bitset_create: Chyba překladu\n",(velikost) > 0) );\
    unsigned long *jmeno_pole = (unsigned long*) malloc(ARRAY_SIZE(velikost) * LONG_SIZE);\
    if(!(jmeno_pole)){\
        error_exit("bitset_alloc: Chyba alokace paměti\n");\
    }\
    (jmeno_pole)[0] = (velikost)

#ifndef USE_INLINE

//Uvolnění alokovaného pole
#define bitset_free(jmeno_pole)\
    free(jmeno_pole)

//Vrácení velikosti pole
#define bitset_size(jmeno_pole)\
    (jmeno_pole)[0]

//Vynuluje nebo nastaví na 1 celý obsah pole
#define bitset_fill(jmeno_pole, bool_výraz)\
        ((bool_výraz) ?\
        memset(&jmeno_pole[1],0xFF,(jmeno_pole[0] / CHAR_BIT + 1)) :\
        memset(&jmeno_pole[1],0x00,(jmeno_pole[0] / CHAR_BIT + 1)))\

//Nastavi bit na pozici 'index' bitoveho pole na hodnotu 1 nebo 0
#define bitset_setbit(jmeno_pole,index,bool_výraz)\
        ((index) > jmeno_pole[0]) ?\
        error_exit("bitset_setbit: Index %lu mimo rozsah 0..%lu\n", (unsigned long) (index), (unsigned long)jmeno_pole[0]), 1 :\
        ((bool_výraz) ?\
        (jmeno_pole[ARRAY_INDEX(index)] |= (BIT_MASK(index))) :\
        (jmeno_pole[ARRAY_INDEX(index)] &= ~(BIT_MASK(index))))\

//Vraci hodnotu bitu na pozici 'index' bitoveho pole
#define bitset_getbit(jmeno_pole,index)\
        ((index) > (bitset_size(jmeno_pole))) ?\
        error_exit("bitset_getbit: Index %lu mimo rozsah 0..%lu\n", (unsigned long) (index), (unsigned long)jmeno_pole[0]), 1 :\
        ((jmeno_pole[ARRAY_INDEX(index)] & (BIT_MASK(index))) > 0 )\

#else

//Uvolnění alokovaného pole
inline void bitset_free(bitset_t jmeno_pole){
    free(jmeno_pole);
}

//Vrácení velikosti pole
inline bitset_index_t bitset_size(bitset_t jmeno_pole){
    return jmeno_pole[0];
}

//Vynuluje nebo nastaví na 1 celý obsah pole
inline void bitset_fill(bitset_t jmeno_pole,bool bool_výraz){
    bool_výraz ?
        memset(&jmeno_pole[1],0xFF,(jmeno_pole[0] / CHAR_BIT + 1)):
        memset(&jmeno_pole[1],0x00,(jmeno_pole[0] / CHAR_BIT + 1));
}

//Nastavi bit na pozici 'index' bitoveho pole na hodnotu 1 nebo 0
inline void bitset_setbit(bitset_t jmeno_pole,bitset_index_t index,bool bool_výraz){
        if(index > jmeno_pole[0])
            error_exit("bitset_setbit: Index %lu mimo rozsah 0..%lu\n",index,jmeno_pole[0]);
        bool_výraz ?
            (jmeno_pole[ARRAY_INDEX(index)] |= (BIT_MASK(index))):
            (jmeno_pole[ARRAY_INDEX(index)] &= ~(BIT_MASK(index)));
}

//Vraci hodnotu bitu na pozici 'index' bitoveho pole
inline bool bitset_getbit(bitset_t jmeno_pole,bitset_index_t index){
        if(index > jmeno_pole[0])
            error_exit("bitset_getbit: Index %lu mimo rozsah 0..%lu\n",index,jmeno_pole[0]);
        return ((jmeno_pole[ARRAY_INDEX(index)] & (BIT_MASK(index))) > 0);
}


#endif
#endif