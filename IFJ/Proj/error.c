// Project: Implementation of a compiler for the IFJ24 imperative language.
// Author: František Sedláček ( xsedla2d )

#include "error.h"

const char* const ERROR_STRINGS[] = {
    "NO_ERR",                // 0
    "LEXICAL_ERR",           // 1
    "SYNTACTIC_ERR",         // 2
    "SEM_UNDEFINED_ERR",     // 3
    "SEM_FUNCTION_ERR",      // 4
    "SEM_VARIABLE_ERR",      // 5
    "SEM_RETURN_ERR",        // 6
    "SEM_COMPATIBILITY_ERR", // 7
    "SEM_INFERENCE_ERR",     // 8
    "SEM_UNUSED_ERR",        // 9
    "SEM_OTHER",             // 10
    [99] = "INTERNAL_ERR"    // 99
};
