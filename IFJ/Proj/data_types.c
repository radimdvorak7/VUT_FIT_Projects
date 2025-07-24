// Projekt: Implementace překladače imperativního jazyka IFJ24
// Autor: Marek Slaný   (xslany03)

#include "data_types.h"
#include <stdbool.h>
const char* const DATA_TYPE_STRINGS[] = {
    "I32",           "F64",         "U8",
    "NULL",          "U8_SLICE",    "I32_OPTIONAL",
    "F64_OPTIONAL",  "U8_OPTIONAL", "U8_SLICE_OPTIONAL",
    "BOOLEAN",       "UNKNOWN",     "VOID",
    "STRING_LITERAL"};

// Returns 'true' if type is an optional, otherwise 'false'
bool type_is_optional(data_type d) {
    if (d == U8_OPTIONAL || d == I32_OPTIONAL || d == U8_SLICE_OPTIONAL ||
        d == F64_OPTIONAL)
        return true;
    return false;
}

// Returns true if expression of type 'd' can be assigned to a variable
bool type_is_assignable(data_type d) {
    if (type_is_optional(d) || d == U8 || d == I32 || d == U8_SLICE ||
        d == F64 || d == DT_NULL) {
        return true;
    }
    return false;
}

// Returns true if type 'd' can be converted (i32, f64).
bool type_is_convertible(data_type d) {
    if (d == I32 || d == F64) {
        return true;
    }
    return false;
}

// Returns the unwrapped type contained inside the option or 'UNKNOWN' if the
// passed type is not optional.
data_type unwrap_optional(data_type d) {
    switch (d) {
    case U8_OPTIONAL:
        return U8;
    case U8_SLICE_OPTIONAL:
        return U8_SLICE;
    case I32_OPTIONAL:
        return I32;
    case F64_OPTIONAL:
        return F64;
    default:
        return UNKNOWN;
    }
}
