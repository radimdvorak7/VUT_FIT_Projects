/*
 * Tabulka s rozptýlenými položkami
 *
 * S využitím datových typů ze souboru hashtable.h a připravených koster
 * funkcí implementujte tabulku s rozptýlenými položkami s explicitně
 * zretězenými synonymy.
 *
 * Při implementaci uvažujte velikost tabulky HT_SIZE.
 */

#include "hashtable.h"
#include <stdlib.h>
#include <string.h>

int HT_SIZE = MAX_HT_SIZE;

/*
 * Rozptylovací funkce která přidělí zadanému klíči index z intervalu
 * <0,HT_SIZE-1>. Ideální rozptylovací funkce by měla rozprostírat klíče
 * rovnoměrně po všech indexech. Zamyslete sa nad kvalitou zvolené funkce.
 */
int get_hash(char *key) {
  int result = 1;
  int length = strlen(key);
  for (int i = 0; i < length; i++) {
    result += key[i];
  }
  return (result % HT_SIZE);
}

/*
 * Inicializace tabulky — zavolá sa před prvním použitím tabulky.
 */
void ht_init(ht_table_t *table) {
  for(int i = 0; i < HT_SIZE; i++){
    (*table)[i] = NULL;
  }
}

/*
 * Vyhledání prvku v tabulce.
 *
 * V případě úspěchu vrací ukazatel na nalezený prvek; v opačném případě vrací
 * hodnotu NULL.
 */
ht_item_t *ht_search(ht_table_t *table, char *key) {
  // Získání seznamu synonym pro daný klíč
  int index = get_hash(key);
  ht_item_t* hash_item = (*table)[index];

  // Nachází se prvek s daným v tabulce
  while(hash_item != NULL){
    if(!strcmp(hash_item->key,key)){
      return hash_item;
    }
    else{
      hash_item = hash_item->next;
    }
  }
  return NULL;
}

/*
 * Vložení nového prvku do tabulky.
 *
 * Pokud prvek s daným klíčem už v tabulce existuje, nahraďte jeho hodnotu.
 *
 * Při implementaci využijte funkci ht_search. Pri vkládání prvku do seznamu
 * synonym zvolte nejefektivnější možnost a vložte prvek na začátek seznamu.
 */
void ht_insert(ht_table_t *table, char *key, float value) {
  // Získání prvku
  ht_item_t* item = ht_search(table,key);
  // Získání indexu tabulky
  int index = get_hash(key);

  // Pokud není prvek v tabulce
  if(item == NULL){
    // Alokace prvku
    item = (ht_item_t*) malloc(sizeof(ht_item_t));
    if(item == NULL) return;
    // Inicializace prvku
    item->key = key;
    item->value = value;
    item->next = NULL;
    // Vložení do neprázdného seznamu synonym
    if((*table)[index] != NULL){
      ht_item_t* tmp = (*table)[index];
      (*table)[index] = item;
      item->next = tmp;
    }
    // Vložení do prázdného seznamu synonym
    else {
      (*table)[index] = item;
    }
  }
  // Změna hodnoty pokud je prvek v tabulce
  else {
    item->value = value;
  }
}

/*
 * Získání hodnoty z tabulky.
 *
 * V případě úspěchu vrací funkce ukazatel na hodnotu prvku, v opačném
 * případě hodnotu NULL.
 *
 * Při implementaci využijte funkci ht_search.
 */
float *ht_get(ht_table_t *table, char *key) {
  // Nalézá se klíč v tabulce
  ht_item_t* item = ht_search(table,key);

  // Jestli nalézá vrat jeho hodnotu
  if(item != NULL){
    return &(item->value);
  }

  return NULL;
}

/*
 * Smazání prvku z tabulky.
 *
 * Funkce korektně uvolní všechny alokované zdroje přiřazené k danému prvku.
 * Pokud prvek neexistuje, funkce nedělá nic.
 *
 * Při implementaci NEPOUŽÍVEJTE funkci ht_search.
 */
void ht_delete(ht_table_t *table, char *key) {
  // Získání prvku
  int index = get_hash(key);

  ht_item_t* item = (*table)[index];
  ht_item_t* tmp = NULL;

  // Projití seznamu synonym dokud nenarazí na prvek s daným klíčem
  while(item != NULL){
    // Klíč prvku a klíč jsou stejné
    if(!strcmp(item->key,key)){
      // První prvek
      if(tmp == NULL){
        // Uvolnění prvku
        tmp = item;
        item = item->next;
        free(tmp);
        return;
      }
      else{
        // Uvolnění prvku
        item = item->next;
        free(tmp->next);
        tmp->next = item;
        return;
      }
    }
    // Přeskočení prvku
    tmp = item;
    item = item->next;
  }
}

/*
 * Smazání všech prvků z tabulky.
 *
 * Funkce korektně uvolní všechny alokované zdroje a uvede tabulku do stavu po 
 * inicializaci.
 */
void ht_delete_all(ht_table_t *table) {
  // Pro všechny položky tabulky
  for(int i = 0; i < HT_SIZE; i++){
    ht_item_t* tmp = (*table)[i];
    // Projdi seznam synonym
    while((*table)[i] != NULL){
      // Uvolni prvek
      tmp = (*table)[i]->next;
      free((*table)[i]->next);
      (*table)[i] = tmp;
    }
    (*table)[i] = NULL;
  }
}
