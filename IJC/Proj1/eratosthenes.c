/* eratosthenes.c
 * Řešení IJC-DU1, příklad a), 26.03.2024
 * Autor: Radim Dvořák, xdvorar00, FIT
 * Přeloženo: gcc 11.4.0
 * bitset_setbit na bitu 0 porovnává 0 s unsigned hodnotou
 */

#include <stdio.h>
#include <math.h>
#include "bitset.h"
#include "error.h"
#include "eratosthenes.h"

void Eratosthenes(bitset_t pole){
    const bitset_index_t n = bitset_size(pole);
    const bitset_index_t odmocnina = sqrt(n);

    //vyplnění pole 1
    bitset_fill(pole,1);

    //nastavení bitu 0 a 1 na nulu

    //bitset_setbit(pole, 0, 0) porovnává 0 s unsigned hodnotou
    //bitset_setbit(pole, 0, 0);
    bitset_setbit(pole, 1, 0);

    for(bitset_index_t i = 2; i < odmocnina ; i++){
        if(bitset_getbit(pole,i)){
            for(bitset_index_t j = 2*i; j < n;j += i){
                bitset_setbit(pole,j,0);
            }
        }
    }
}