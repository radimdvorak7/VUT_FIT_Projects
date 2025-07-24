// Project: Implementation of a compiler for the IFJ24 imperative language.
// Author: František Sedláček ( xsedla2d )

#ifndef IFJ_2024_ERROR_H
#define IFJ_2024_ERROR_H

typedef enum {
    NO_ERR = 0,
    LEXICAL_ERR = 1, // chyba v programu v rámci lexikální analýzy (chybná
                     // struktura aktuálního lexému).
    SYNTACTIC_ERR = 2, // chyba v programu v rámci syntaktické analýzy (chybná
                       // syntaxe programu, chybějící hlavička, atp.).
    SEM_UNDEFINED_ERR =
        3, // sémantická chyba v programu – nedefinovaná funkce či proměnná.
    SEM_FUNCTION_ERR = 4, // sémantická chyba v programu – špatný počet/typ
                          // parametrů u volání funkce; špatný typ či nepovolené
                          // zahození návratové hodnoty z funkce.
    SEM_VARIABLE_ERR =
        5, // sémantická chyba v programu – redefinice proměnné nebo funkce;
           // přiřazení do nemodifikovatelné proměnné.
    SEM_RETURN_ERR = 6, // sémantická chyba v programu – chybějící/přebývající
                        // výraz v příkazu návratu z funkce.
    SEM_COMPATIBILITY_ERR =
        7, // sémantická chyba typové kompatibility v aritmetických, řetězcových
           // a relačních výrazech; nekompatibilní typ výrazu (např. při
           // přiřazení).
    SEM_INFERENCE_ERR = 8, // sémantická chyba odvození typu – typ proměnné není
                           // uveden a nelze odvodit od použitého výrazu.
    SEM_UNUSED_ERR =
        9, // sémantická chyba nevyužité proměnné v jejím rozsahu platnosti;
           // modifikovatelná proměnná bez možnosti změny po její inicializaci.
    SEM_OTHER = 10,    // ostatní sémantické chyby.
    INTERNAL_ERR = 99, // interní chyba překladače tj. neovlivněná vstupním
                       // programem (např. chyba alokace paměti atd.).
} error_code;

extern const char* const ERROR_STRINGS[];

#ifndef NDEBUG
#define DEBUG_PRINT(fmt, ...)                                                  \
    fprintf(stderr,                                                            \
            "\x1b[2m" fmt "\x1b[0m"                                            \
            "\n",                                                              \
            __VA_ARGS__);

#define ERROR_PRINT(fmt, ...)                                                  \
    fprintf(stderr,                                                            \
            "\x1b[31m" fmt "\x1b[0m"                                           \
            "\n",                                                              \
            __VA_ARGS__);
#else
#define DEBUG_PRINT(fmt, ...)
#define ERROR_PRINT(fmt, ...)
#endif // NDEBUG

#endif // IFJ_2024_ERROR_H
