/* no-comment.c
 * Řešení IJC-DU1, příklad b), 26.03.2024
 * Autor: Radim Dvořák, xdvorar00, FIT
 * Přeloženo: gcc 11.4.0
 */

#include <stdlib.h>
#include <stdio.h>
#include "error.h"

int main(int argc, char *argv[]){
    FILE *in;

    //otevření souboru pokud je v argumentu
    if(argc > 1){
        in = fopen(argv[1],"r");
        if( in == NULL ){
            error_exit("Soubor nelze otevřít pro čtení\n", in);
        }

    }
    //když nebude uveden soubor čtení bude probíhat z stdin
    else{
        in = stdin;
    }

    int stav = 0;
    int c;
    //projití vstupního souboru
    while((c = getc(in)) != EOF){
        switch (stav){
        case 0:
            if(c == '\''){
                putchar(c);
                stav = 7;
            }
            else if(c == '/'){
                stav = 1;
            }
            else if(c == '\"'){
                putchar('\"');
                stav = 5;
            }
            else{
                putchar(c);
            }
            break;
        case 1:
            if(c == '*'){
                stav = 2;
            }
            else if (c == '/')
            {
                stav = 4;
            }
            else{
                stav = 0;
                putchar('/');
                putchar(c);
            }
            break;
        case 2:
            if(c =='*'){
                stav = 3;
            }
            break;
        case 3:
            if(c == '/'){
                stav = 0;
                putchar(' ');
            }
            else if(c != '*'){
                stav = 2;
            }
            break;
        case 4:
            if(c =='\n'){
                stav = 0;
                putchar('\n');
            }
            break;
        case 5:
            if(c == '"'){
                stav = 0;
                putchar('"');
            }
            else if(c =='\\'){
                stav = 6;
                putchar('\\');
            }
            else{
                putchar(c);
            }
            break;
        case 6:
            stav = 5;
            putchar(c);
            break;
        case 7:
            if(c =='\''){
                stav = 0;
                putchar('\'');
            }
            else if(c =='\\'){
                stav = 8;
                putchar('\\');
            }
            else{
                putchar(c);
            }
            break;
        case 8:
            stav = 7;
            putchar(c);
            break;
        }
    }
    fclose(in);
    if(stav != 0){
        error_exit("Chyba čtení souboru\n");
    }
    return 0;
}