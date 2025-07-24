/* primes.c
 * Řešení IJC-DU1, příklad a), 26.03.2024
 * Autor: Radim Dvořák, xdvorar00, FIT
 * Přeloženo: gcc 11.4.0
 */

#include <time.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#include "error.h"
#include "bitset.h"
#include "eratosthenes.h"

#define NUM_BITS 666000000UL 
#define NUM_OF_PRIMES 10UL

int main(void){
    clock_t start = clock();

    bitset_create(pole,NUM_BITS);//vytvoří pole vhodné velikosti
    Eratosthenes(pole);//Projití polem pomocí Eratostenovo síto
    //Najití posledních 10 prvočísel od konce pole
    for (bitset_index_t i = pole[0], j = 0; j < 11; --i)
    {   
        if (bitset_getbit(pole, i)){
            j++;
        }
        //Při najití jedenáctého prvočísla, vypisuju předchozích 10 prvočísel
        if (j == 11){
            for (; i < pole[0]; ++i){
                if (bitset_getbit(pole, i)){
                    printf("%lu\n",i);
                }
            }
        }
    }
    fprintf(stderr, "Time=%.3g\n", (double)(clock()-start)/CLOCKS_PER_SEC);
    return 0;
}