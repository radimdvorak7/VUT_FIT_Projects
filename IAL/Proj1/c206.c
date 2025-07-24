/* ******************************* c206.c *********************************** */
/*  Předmět: Algoritmy (IAL) - FIT VUT v Brně                                 */
/*  Úkol: c206 - Dvousměrně vázaný lineární seznam                            */
/*  Návrh a referenční implementace: Bohuslav Křena, říjen 2001               */
/*  Vytvořil: Martin Tuček, říjen 2004                                        */
/*  Upravil: Kamil Jeřábek, září 2020                                         */
/*           Daniel Dolejška, září 2021                                       */
/*           Daniel Dolejška, září 2022                                       */
/* ************************************************************************** */
/*
** Implementujte abstraktní datový typ dvousměrně vázaný lineární seznam.
** Užitečným obsahem prvku seznamu je hodnota typu int. Seznam bude jako datová
** abstrakce reprezentován proměnnou typu DLList (DL znamená Doubly-Linked
** a slouží pro odlišení jmen konstant, typů a funkcí od jmen u jednosměrně
** vázaného lineárního seznamu). Definici konstant a typů naleznete
** v hlavičkovém souboru c206.h.
**
** Vaším úkolem je implementovat následující operace, které spolu s výše
** uvedenou datovou částí abstrakce tvoří abstraktní datový typ obousměrně
** vázaný lineární seznam:
**
**      DLL_Init ........... inicializace seznamu před prvním použitím,
**      DLL_Dispose ........ zrušení všech prvků seznamu,
**      DLL_InsertFirst .... vložení prvku na začátek seznamu,
**      DLL_InsertLast ..... vložení prvku na konec seznamu,
**      DLL_First .......... nastavení aktivity na první prvek,
**      DLL_Last ........... nastavení aktivity na poslední prvek,
**      DLL_GetFirst ....... vrací hodnotu prvního prvku,
**      DLL_GetLast ........ vrací hodnotu posledního prvku,
**      DLL_DeleteFirst .... zruší první prvek seznamu,
**      DLL_DeleteLast ..... zruší poslední prvek seznamu,
**      DLL_DeleteAfter .... ruší prvek za aktivním prvkem,
**      DLL_DeleteBefore ... ruší prvek před aktivním prvkem,
**      DLL_InsertAfter .... vloží nový prvek za aktivní prvek seznamu,
**      DLL_InsertBefore ... vloží nový prvek před aktivní prvek seznamu,
**      DLL_GetValue ....... vrací hodnotu aktivního prvku,
**      DLL_SetValue ....... přepíše obsah aktivního prvku novou hodnotou,
**      DLL_Previous ....... posune aktivitu na předchozí prvek seznamu,
**      DLL_Next ........... posune aktivitu na další prvek seznamu,
**      DLL_IsActive ....... zjišťuje aktivitu seznamu.
**
** Při implementaci jednotlivých funkcí nevolejte žádnou z funkcí
** implementovaných v rámci tohoto příkladu, není-li u funkce explicitně
** uvedeno něco jiného.
**
** Nemusíte ošetřovat situaci, kdy místo legálního ukazatele na seznam
** předá někdo jako parametr hodnotu NULL.
**
** Svou implementaci vhodně komentujte!
**
** Terminologická poznámka: Jazyk C nepoužívá pojem procedura.
** Proto zde používáme pojem funkce i pro operace, které by byly
** v algoritmickém jazyce Pascalovského typu implemenovány jako procedury
** (v jazyce C procedurám odpovídají funkce vracející typ void).
**
**/

#include "c206.h"

bool error_flag;
bool solved;

/**
 * Vytiskne upozornění na to, že došlo k chybě.
 * Tato funkce bude volána z některých dále implementovaných operací.
 */
void DLL_Error(void) {
	printf("*ERROR* The program has performed an illegal operation.\n");
	error_flag = true;
}

/**
 * Provede inicializaci seznamu list před jeho prvním použitím (tzn. žádná
 * z následujících funkcí nebude volána nad neinicializovaným seznamem).
 * Tato inicializace se nikdy nebude provádět nad již inicializovaným seznamem,
 * a proto tuto možnost neošetřujte.
 * Vždy předpokládejte, že neinicializované proměnné mají nedefinovanou hodnotu.
 *
 * @param list Ukazatel na strukturu dvousměrně vázaného seznamu
 */
void DLL_Init( DLList *list ) {
	// Vyresetování všech hodnot
	list->currentLength = 0;
	list->activeElement = NULL;
	list->firstElement = NULL;
	list->lastElement = NULL;
}

/**
 * Zruší všechny prvky seznamu list a uvede seznam do stavu, v jakém se nacházel
 * po inicializaci.
 * Rušené prvky seznamu budou korektně uvolněny voláním operace free.
 *
 * @param list Ukazatel na inicializovanou strukturu dvousměrně vázaného seznamu
 */
void DLL_Dispose( DLList *list ) {
	DLLElementPtr list_element = list->firstElement;

	// Projití všech prvků a jejich uvolnění 
	while(list_element != NULL){
		DLLElementPtr tmp = list_element->nextElement;
		free(list_element);
		list_element = tmp;
	}

	// Vyresetování všech hodnot
	list->activeElement = NULL;
	list->firstElement = NULL;
	list->lastElement = NULL;
	list->currentLength = 0;
}

/**
 * Vloží nový prvek na začátek seznamu list.
 * V případě, že není dostatek paměti pro nový prvek při operaci malloc,
 * volá funkci DLL_Error().
 *
 * @param list Ukazatel na inicializovanou strukturu dvousměrně vázaného seznamu
 * @param data Hodnota k vložení na začátek seznamu
 */
void DLL_InsertFirst( DLList *list, long data ) {
	// Alokování nového prvku seznamu
	DLLElementPtr new = malloc(sizeof(struct DLLElement));
	if(new == NULL){
		DLL_Error();
		return;
	}

	// Nastavení dat nového prvku
	new->data = data;
	
	// Nastavení odkazů z nového prvku
	new->nextElement = list->firstElement;
	new->previousElement = NULL;

	// Není-li seznam prázdný nastav předchozí prvek z prvního, jinak nastav nový prvek jako poslední
	if(list->firstElement != NULL){
		list->firstElement->previousElement = new;
	}
	else{
		list->lastElement = new;
	}

	// Nastavení prvního prvku na nový
	list->firstElement = new;
	list->currentLength++;
}

/**
 * Vloží nový prvek na konec seznamu list (symetrická operace k DLL_InsertFirst).
 * V případě, že není dostatek paměti pro nový prvek při operaci malloc,
 * volá funkci DLL_Error().
 *
 * @param list Ukazatel na inicializovanou strukturu dvousměrně vázaného seznamu
 * @param data Hodnota k vložení na konec seznamu
 */
void DLL_InsertLast( DLList *list, long data ) {
	// Alokování nového prvku seznamu
	DLLElementPtr new = malloc(sizeof(struct DLLElement));
	if(new == NULL){
		DLL_Error();
		return;
	}

	// Nastavení dat nového prvku
	new->data = data;

	// Nastavení odkazů z nového prvku
	new->previousElement = list->lastElement;
	new->nextElement = NULL;

	// Není-li seznam prázdný nastav následujíci prvek z posledního, jinak nastav nový prvek jako první
	if(list->lastElement != NULL){
		list->lastElement->nextElement = new;
	}
	else{
		list->firstElement = new;
	}

	// Nastavení posledního prvku na nový
	list->lastElement = new;
	list->currentLength++;
}

/**
 * Nastaví první prvek seznamu list jako aktivní.
 * Funkci implementujte jako jediný příkaz, aniž byste testovali,
 * zda je seznam list prázdný.
 *
 * @param list Ukazatel na inicializovanou strukturu dvousměrně vázaného seznamu
 */
void DLL_First( DLList *list ) {
	list->activeElement = list->firstElement;
}

/**
 * Nastaví poslední prvek seznamu list jako aktivní.
 * Funkci implementujte jako jediný příkaz, aniž byste testovali,
 * zda je seznam list prázdný.
 *
 * @param list Ukazatel na inicializovanou strukturu dvousměrně vázaného seznamu
 */
void DLL_Last( DLList *list ) {
	list->activeElement = list->lastElement;
}

/**
 * Prostřednictvím parametru dataPtr vrátí hodnotu prvního prvku seznamu list.
 * Pokud je seznam list prázdný, volá funkci DLL_Error().
 *
 * @param list Ukazatel na inicializovanou strukturu dvousměrně vázaného seznamu
 * @param dataPtr Ukazatel na cílovou proměnnou
 */
void DLL_GetFirst( DLList *list, long *dataPtr ) {
	// Když je seznam prázdný nedělá nic
	if(list->firstElement == NULL){
		DLL_Error();
		return;
	}

	// Získej data z prvního prvku
	*dataPtr = list->firstElement->data;
}

/**
 * Prostřednictvím parametru dataPtr vrátí hodnotu posledního prvku seznamu list.
 * Pokud je seznam list prázdný, volá funkci DLL_Error().
 *
 * @param list Ukazatel na inicializovanou strukturu dvousměrně vázaného seznamu
 * @param dataPtr Ukazatel na cílovou proměnnou
 */
void DLL_GetLast( DLList *list, long *dataPtr ) {
	// Když je seznam prázdný nedělá nic
	if(list->lastElement == NULL){
		DLL_Error();
		return;
	}

	// Získej data z posledního prvku
	*dataPtr = list->lastElement->data;
}

/**
 * Zruší první prvek seznamu list.
 * Pokud byl první prvek aktivní, aktivita se ztrácí.
 * Pokud byl seznam list prázdný, nic se neděje.
 *
 * @param list Ukazatel na inicializovanou strukturu dvousměrně vázaného seznamu
 */
void DLL_DeleteFirst( DLList *list ) {
	// Když je seznam prázdný nedělá nic
	if(list->firstElement == NULL){
		return;
	}

	// Zachování odkazu na rušený prvek
	DLLElementPtr tmp = list->firstElement;

	// Aktivní prvek je první, ztrácí se aktivita
	if(list->activeElement == list->firstElement){
		list->activeElement = NULL;
	}

	// Seznam má pouze jeden prvek
	if(list->firstElement == list->lastElement){
		list->firstElement = NULL;
		list->lastElement = NULL;
	}
	// Nastav odkazy z prvního prvku
	else{
		list->firstElement = list->firstElement->nextElement;
		list->firstElement->previousElement = NULL;
	}

	free(tmp);
	list->currentLength--;
}

/**
 * Zruší poslední prvek seznamu list.
 * Pokud byl poslední prvek aktivní, aktivita seznamu se ztrácí.
 * Pokud byl seznam list prázdný, nic se neděje.
 *
 * @param list Ukazatel na inicializovanou strukturu dvousměrně vázaného seznamu
 */
void DLL_DeleteLast( DLList *list ) {
	if(list->lastElement == NULL){
		return;
	}

	// Zachování odkazu na rušený prvek
	DLLElementPtr tmp = list->lastElement;

	// Aktivní prvek je poslední, ztrácí se aktivita
	if(list->activeElement == list->lastElement){
		list->activeElement = NULL;
	}

	// Seznam má pouze jeden prvek
	if(list->firstElement == list->lastElement){
		list->firstElement = NULL;
		list->lastElement = NULL;
	}
	// Nastav odkazy z posledního prvku
	else{
		list->lastElement = list->lastElement->previousElement;
		list->lastElement->nextElement = NULL;
	}

	free(tmp);
	list->currentLength--;
}

/**
 * Zruší prvek seznamu list za aktivním prvkem.
 * Pokud je seznam list neaktivní nebo pokud je aktivní prvek
 * posledním prvkem seznamu, nic se neděje.
 *
 * @param list Ukazatel na inicializovanou strukturu dvousměrně vázaného seznamu
 */
void DLL_DeleteAfter( DLList *list ) {
	// Když je seznam neaktivní nebo aktivní prvek je poslední nedělá nic
	if(list->activeElement == NULL || list->activeElement == list->lastElement){
		return;
	}

	// Zachování odkazu na rušený prvek
	DLLElementPtr tmp = list->activeElement->nextElement;
	
	// Přeskočení rušeného prvku
	list->activeElement->nextElement = tmp->nextElement;

	// Rušený prvek je poslední, nastav poslední prvek na aktvní
	if (tmp == list->lastElement){
		list->lastElement = list->activeElement;
	}

	// Uvolnění paměti a snížení počtu prvků
	list->currentLength--;
	free(tmp);
}
	

/**
 * Zruší prvek před aktivním prvkem seznamu list .
 * Pokud je seznam list neaktivní nebo pokud je aktivní prvek
 * prvním prvkem seznamu, nic se neděje.
 *
 * @param list Ukazatel na inicializovanou strukturu dvousměrně vázaného seznamu
 */
void DLL_DeleteBefore( DLList *list ) {
	// Když je seznam neaktivní nebo aktivní prvek je první nedělá nic
	if(list->activeElement == NULL || list->activeElement == list->firstElement){
		return;
	}
	// Zachování odkazu na rušený prvek
	DLLElementPtr tmp = list->activeElement->previousElement;
	
	// Přeskočení rušeného prvku
	list->activeElement->previousElement = tmp->previousElement;

	// Rušený prvek je první, nastav první prvek na aktvní
	if (tmp == list->firstElement){
		list->firstElement = list->activeElement;
	}

	// Uvolnění paměti a snížení počtu prvků
	list->currentLength--;
	free(tmp);
}

/**
 * Vloží prvek za aktivní prvek seznamu list.
 * Pokud nebyl seznam list aktivní, nic se neděje.
 * V případě, že není dostatek paměti pro nový prvek při operaci malloc,
 * volá funkci DLL_Error().
 *
 * @param list Ukazatel na inicializovanou strukturu dvousměrně vázaného seznamu
 * @param data Hodnota k vložení do seznamu za právě aktivní prvek
 */
void DLL_InsertAfter( DLList *list, long data ) {
	// Když je seznam neaktivní nedělá nic
	if (list->activeElement == NULL){
		return;
	}

	// Alokování nového prvku
	DLLElementPtr newElement = (DLLElementPtr) malloc(sizeof(struct DLLElement));
	if(newElement == NULL){
		DLL_Error();
		return;
	}

	// Nastavení dat nového prvku
	newElement->data = data;

	// Nastavení ukazatelů z nového prvku
	newElement->nextElement = list->activeElement->nextElement;
	newElement->previousElement = list->activeElement;
	list->activeElement->nextElement = newElement;

	// Když aktvni prvek byl poslední, posunutů posledního prvku
	if (list->activeElement == list->lastElement){
		list->lastElement = newElement;
	}

	list->currentLength++;
}

/**
 * Vloží prvek před aktivní prvek seznamu list.
 * Pokud nebyl seznam list aktivní, nic se neděje.
 * V případě, že není dostatek paměti pro nový prvek při operaci malloc,
 * volá funkci DLL_Error().
 *
 * @param list Ukazatel na inicializovanou strukturu dvousměrně vázaného seznamu
 * @param data Hodnota k vložení do seznamu před právě aktivní prvek
 */
void DLL_InsertBefore( DLList *list, long data ) {
	// Když je seznam neaktivní nedělá nic
	if (list->activeElement == NULL){
		return;
	}

	// Alokování nového prvku
	DLLElementPtr newElement = (DLLElementPtr) malloc(sizeof(struct DLLElement));
	if(newElement == NULL){
		DLL_Error();
		return;
	}

	// Nastavení dat nového prvku
	newElement->data = data;

	// Nastavení ukazatelů z nového prvku
	newElement->nextElement = list->activeElement;
	newElement->previousElement = list->activeElement->previousElement;
	list->activeElement->previousElement = newElement;

	// Když aktvni prvek byl první, posunutů prvního prvku
	if (list->activeElement == list->firstElement){
		list->firstElement = newElement;
	}
	list->currentLength++;
}

/**
 * Prostřednictvím parametru dataPtr vrátí hodnotu aktivního prvku seznamu list.
 * Pokud seznam list není aktivní, volá funkci DLL_Error ().
 *
 * @param list Ukazatel na inicializovanou strukturu dvousměrně vázaného seznamu
 * @param dataPtr Ukazatel na cílovou proměnnou
 */
void DLL_GetValue( DLList *list, long *dataPtr ) {
	// Když je seznam neaktivní nedělá nic
	if(list->activeElement == NULL){
		DLL_Error();
		return;
	}

	// Získej data z prvku
	*dataPtr = list->activeElement->data;
}

/**
 * Přepíše obsah aktivního prvku seznamu list.
 * Pokud seznam list není aktivní, nedělá nic.
 *
 * @param list Ukazatel na inicializovanou strukturu dvousměrně vázaného seznamu
 * @param data Nová hodnota právě aktivního prvku
 */
void DLL_SetValue( DLList *list, long data ) {
	// Když je seznam neaktivní nedělá nic
	if(list->activeElement == NULL){
		return;
	}
	// Ulož data do prvku
	list->activeElement->data = data;
	return;
}

/**
 * Posune aktivitu na následující prvek seznamu list.
 * Není-li seznam aktivní, nedělá nic.
 * Všimněte si, že při aktivitě na posledním prvku se seznam stane neaktivním.
 *
 * @param list Ukazatel na inicializovanou strukturu dvousměrně vázaného seznamu
 */
void DLL_Next( DLList *list ) {
	// Když je seznam neaktivní nedělá nic
	if(list->activeElement == NULL){
		return;
	}

	// Posunutí aktivního prvku na následující
	list->activeElement = list->activeElement->nextElement;
}


/**
 * Posune aktivitu na předchozí prvek seznamu list.
 * Není-li seznam aktivní, nedělá nic.
 * Všimněte si, že při aktivitě na prvním prvku se seznam stane neaktivním.
 *
 * @param list Ukazatel na inicializovanou strukturu dvousměrně vázaného seznamu
 */
void DLL_Previous( DLList *list ) {
	// Když je seznam neaktivní nedělá nic
	if(list->activeElement == NULL){
		return;
	}

	// Posunutí aktivního prvku na předchozí
	list->activeElement = list->activeElement->previousElement;
}

/**
 * Je-li seznam list aktivní, vrací nenulovou hodnotu, jinak vrací 0.
 * Funkci je vhodné implementovat jedním příkazem return.
 *
 * @param list Ukazatel na inicializovanou strukturu dvousměrně vázaného seznamu
 *
 * @returns Nenulovou hodnotu v případě aktivity prvku seznamu, jinak nulu
 */
bool DLL_IsActive( DLList *list ) {
	return list->activeElement != NULL;
}

/* Konec c206.c */
