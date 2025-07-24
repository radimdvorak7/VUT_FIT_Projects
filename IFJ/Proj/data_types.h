// Projekt: Implementace překladače imperativního jazyka IFJ24
// Autor: Marek Slaný   (xslany03)
//
// Definition of data types in the ifj24 language, and some helper pseudo types
// and functions.

#ifndef DATA_TYPES_H
#define DATA_TYPES_H
#include <stdbool.h>

typedef enum {
    I32,
    F64,
    U8,
    DT_NULL,
    U8_SLICE,
    I32_OPTIONAL,
    F64_OPTIONAL,
    U8_OPTIONAL,
    U8_SLICE_OPTIONAL,
    BOOLEAN,
    UNKNOWN,
    VOID,
    STRING_LITERAL
} data_type;

bool type_is_optional(data_type d);
bool type_is_assignable(data_type d);
data_type unwrap_optional(data_type d);
extern const char* const DATA_TYPE_STRINGS[];

#endif // DATA_TYPES_H
