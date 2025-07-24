/* wordcount.c
 * Řešení IJC-DU2, příklad 2), 25.04.2024
 * Autor: Radim Dvořák, xdvorar00, FIT
 * Přeloženo: gcc 11.4.0
 */

#include <stdlib.h>
#include <stdio.h>
#include "htab.h"
#include "io.h"

#define MAX_WORD_LENGTH 256

void print_pair(htab_pair_t *pair) {
    printf("%s\t%d\n", pair->key, pair->value);
}

int main(){
    /*Neefektivnější se stává hashovací tabulka při zaplnenosti 1.4, proto jsem si vybral velikost 7121,
    což je 1.4-krát průměrný počet slov co má čech ve slovní zásobě a zaokrouhleno na nejbližsí prvočíslo.*/
    htab_t *table = htab_init(7121);
    if(table == NULL){
        fprintf(stderr,"Error: Chyba alokovace paměti\n");
        return 1;
    }
    //Načítání slov
    bool word_correct = true;
    int word_validity;
    char word[MAX_WORD_LENGTH];
    while ((word_validity = read_word(word, MAX_WORD_LENGTH, stdin)) != EOF) {
        //Slovo je moc dlouhé
        if(word_validity == -2){
            if(word_correct){
                word_correct = false;
                fprintf(stderr, "Error: Slovo je delší než %d znaků\n",MAX_WORD_LENGTH);
            }
        }
        //Vložení slova do tabulky
        htab_pair_t *pair = htab_lookup_add(table, word);
        if (pair == NULL) {
            fprintf(stderr, "Error: Failed to add an item to the hash table.\n");
            htab_free(table);
            return 1;
        }
    }
    //Výpis všech záznamů
    htab_for_each(table, print_pair);
    #ifdef STATISTICS
    htab_statistics(table);
    #endif
    htab_free(table);
    return 0;  
}