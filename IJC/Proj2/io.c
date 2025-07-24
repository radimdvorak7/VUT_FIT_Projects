/* io.c
 * Řešení IJC-DU2, příklad 2), 25.04.2024
 * Autor: Radim Dvořák, xdvorar00, FIT
 * Přeloženo: gcc 11.4.0
 */
#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include "io.h"

 int read_word(char *s, int max, FILE *f){
    bool word_correct = true;
    int c;
    int i = 0;

    //Čtení znaku
    while ((c = fgetc(f)) != EOF) {
        if (isspace(c)) {
            if(i > 0){
                break;
            }
            else{
                continue;
            }
        }
        else{
            //Znak se vejde
            if(i < max - 1){
                s[i++] = c;
            }
            //Znak se nevejde
            else{
                if(word_correct){
                    word_correct = false;
                }
            }
        }
        
    }
    //Vložení na konec řetezce
    s[i] = '\0';
    
    if(word_correct == false) return -2;
    return (i > 0 || c != EOF) ? i : EOF;
 }