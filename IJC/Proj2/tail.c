/* tail.c
 * Řešení IJC-DU2, příklad 1), 25.04.2024
 * Autor: Radim Dvořák, xdvorar00, FIT
 * Přeloženo: gcc 11.4.0
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <stdbool.h>
#include <ctype.h>

#define MAX_LINE_LEN 2050 //2048 znaků  + /n + /0

#define MOVE_START(index) ((cb->start + (index)) % cb->capacity)
#define MOVE_END(index) ((cb->end + (index)) % cb->capacity)

typedef struct{
    char **buffer;
    int start;
    int end;
    int size;
    int capacity;
} CircularBuffer;

//Alokuje dostatečně místa a nakopíruje řetezec
char *copy_line(const char* line){
    unsigned int length = strlen(line) + 1;
    char *out = malloc(length * sizeof(char));
    if(out == NULL) return NULL;
    memcpy(out,line,length);
    return out;
}

//Funkce převádí řetezec na číslo
int string2num(const char *str) {
    int i = 0;
    int num = 0;
    int result = 0;
    char ch = str[i++];

    while (ch != '\0') {
        //Není číslice
        if (ch < '0' || ch > '9') {
            return -1;
        }
        //Přičtení číslice
        num = num * 10 + (ch - '0');
        ch = str[i++];
    }

    return result;
}

//Vytváří a alokuje cyklický buffer
CircularBuffer *cbuf_create(int n){
    if(n < 0){
        return NULL;
    }

    //Alokace cyklického bufferu
    CircularBuffer *cb = (CircularBuffer *) malloc(sizeof(CircularBuffer));
    if(cb == NULL) return NULL;
    
    //Nastavení parametrů cyklického bufferu
    cb->capacity = n;
    cb->size = 0;
    cb->start = 0;
    cb->end = 0;

    //Alokace pole ukazatelů
    cb->buffer = (char **) malloc(n * sizeof(char *));
    if (cb->buffer == NULL) {
        free(cb);
        return NULL;
    }
    return cb;
}

//Vkládá řetezec na index end
void cbuf_put(CircularBuffer *cb,char *line){
    //Buffer je plný
    if(cb->size == cb->capacity){
        free(cb->buffer[cb->start]);
        cb->start = MOVE_START(1);
        cb->size--;
    }
    //První vložení
    if(cb->size != 0 || cb->end != 0){
        cb->end = MOVE_END(1);
    }
    //Překopírování řetezce a přiřazení do buferu
    cb->buffer[cb->end] = copy_line(line);
    cb->size++;
}

//Vrací řetezec z indexu start
char *cbuf_get(CircularBuffer *cb){
    char* buf_line = copy_line(cb->buffer[cb->start]);
    if(buf_line == NULL) return NULL;
    free(cb->buffer[cb->start]);
    cb->start = MOVE_START(1);
    cb->size--;
    return buf_line;
}

//Uvolnuje alokovaný buffer
void cbuf_free(CircularBuffer *cb){
    //Uvolní všechny řetezce
    for (int i = 0; i < cb->size; i++)
    {
        free(cb->buffer[MOVE_START(i)]);
    }

    cb->end= 0;
    cb->start = 0;
    cb->size = 0;

    free(cb->buffer);
    free(cb);
}

int main(int argc, char **argv){
    int n = 10;

    FILE *file = stdin;
    if(argc > 4){
        fprintf(stderr,"Error: Moc argumentů: %d\n", argc);
        return 1;
    }

    //Parsování argumentů
    for(int i = 1; i < argc;i++){
        //Počet řádků
        if(!strcmp(argv[i], "-n") && (i + 1 < argc)){
            i++;
            if(string2num(argv[i]) >=0){
                n = atoi(argv[i]);
            }
            else{
                return 1;
            }
            
        }
        //Soubor
        else{
            file = fopen(argv[i],"r");
            if(file == NULL){
                fprintf(stderr,"Error: Nemůžu otevřít soubor: %s\n", argv[i]);
                return 1;
            }
        }
    }

    if(n == 0){
        return 0;
    }
    //Vytvoření bufferu
    CircularBuffer *cb = cbuf_create(n);
    if (cb == NULL) {
        fprintf(stderr, "Error: Nemůžu vytvořit buffer\n");
        return 1;
    }

    //Načtení souboru do bufferu
    char line[MAX_LINE_LEN];
    bool line_correct = true;
    //Načíst maximálně MAX_LINE_LEN znaků
    while (fgets(line, MAX_LINE_LEN, file) != NULL) {
        //Načtený řetezec je delší než dovolený počet znaků
        if ((strlen(line) > MAX_LINE_LEN - 1 || (strlen(line) == MAX_LINE_LEN - 1 && line[strlen(line) - 1] != '\n'))){
            //Výpis chyby při delším řetezci než dovolený počet znaků
            if(line_correct){
                line_correct = false;
                fprintf(stderr, "Error: Řádek je delší než %d znaků\n",MAX_LINE_LEN - 2);
            }
            line[MAX_LINE_LEN - 2] = '\n';
            line[MAX_LINE_LEN - 1] = '\0';
            cbuf_put(cb, line);
            continue;
        }
        cbuf_put(cb, line);
    }
    //Výpis všech položek z bufferu
    for (int i = 0; i < n && cb->size > 0; i++) {
        char *line = cbuf_get(cb);
        if(line == NULL) return 1;
        printf("%s",line);
        free(line);
    }
    
    if(file != stdin) fclose(file);
    cbuf_free(cb);
    return 0;
}