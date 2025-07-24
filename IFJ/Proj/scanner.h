// Project: Implementation of a compiler for the IFJ24 imperative language.
// Author: Radim Dvořák (xdvorar00)
//
// Token declaration and functions for lexical analysis

#ifndef IFJ_PROJEKT_2024_SCANNER_H
#define IFJ_PROJEKT_2024_SCANNER_H

#include "error.h"
#include <stdio.h>

#define T_EOF 0

#define T_KEYWORD 100
#define T_KEYWORD_END 199

#define T_LITERAL 200
#define T_LITERAL_END 299

#define T_OPERATOR 300
#define T_OPERATOR_END 399

#define T_COMP_OP 400
#define T_COMP_OP_END 499

#define T_SYMBOL 500
#define T_SYMBOL_END 599

#define T_IDENTIFIER 1000

typedef enum {

    TOKEN_EOF = 0, // EOF

    // Special Zig keywords
    TOKEN_IMPORT, // @import
    TOKEN_AS,     // @as

    // Key words
    TOKEN_KW_CONST = 100, // const
    TOKEN_KW_ELSE,        // else
    TOKEN_KW_FN,          // fn
    TOKEN_KW_IF,          // if
    TOKEN_KW_INT,         // i32
    TOKEN_KW_INT_NULL,    // ?i32
    TOKEN_KW_FLOAT,       // f64
    TOKEN_KW_FLOAT_NULL,  // ?f64
    TOKEN_KW_NULL,        // null
    TOKEN_KW_PUB,         // pub
    TOKEN_KW_RETURN,      // return
    TOKEN_KW_U8,          // u8
    TOKEN_KW_SLICE,       // []u8
    TOKEN_KW_SLICE_NULL,  // ?[]u8
    TOKEN_KW_VAR,         // var
    TOKEN_KW_VOID,        // void
    TOKEN_KW_WHILE,       // while
    TOKEN_KW_UNREACHABLE, // unreachable
    TOKEN_KW_FOR,         // for
    TOKEN_KW_BREAK,       // break
    TOKEN_KW_CONTINUE,    // continue
    TOKEN_KW_BOOL,        // bool
    TOKEN_KW_TRUE,        // true
    TOKEN_KW_FALSE,       // false

    // Literals
    TOKEN_LIT_INT = 200, // Integer literal
    TOKEN_LIT_FLOAT,     // Float literal
    TOKEN_LIT_STRING,    // String literal

    // Operators
    TOKEN_PLUS = 300,     // +
    TOKEN_MINUS,          // -
    TOKEN_MUL,            // *
    TOKEN_DIV,            // /
    TOKEN_ORELSE,         // orelse
    TOKEN_IS_UNREACHABLE, // ".?" , equivalent to "orelse unreachable"
    TOKEN_NEG,            // !
    TOKEN_AND,            // AND
    TOKEN_OR,             // OR

    // Relational operators
    TOKEN_EQ = 400,     // ==
    TOKEN_NOT_EQ,       // !=
    TOKEN_LESS_THAN,    // <
    TOKEN_GREATER_THAN, // >
    TOKEN_LESS_EQ,      // <=
    TOKEN_GREATER_EQ,   // >=

    // Symbols
    TOKEN_DOT = 500, // .
    TOKEN_COMMA,     // ,
    TOKEN_UNDESCORE, // _
    TOKEN_ASSIGN,    // =
    TOKEN_COLON,     // :
    TOKEN_SEMICOLON, // ;
    TOKEN_VERT_BAR,  // |
    TOKEN_L_PAREN,   // (
    TOKEN_R_PAREN,   // )
    TOKEN_L_CURLY,   // {
    TOKEN_R_CURLY,   // }

    // Identifiers
    TOKEN_IDENT = 1000 // Identifier

} token_type;

extern const char* const TOKEN_STRINGS[];

typedef union {
    int int_lit;
    double float_lit;
    char* string_lit;
    char* id;
} token_content;

typedef struct {
    token_type type;
    token_content content;
} token;

/**
 * Scans standard input and based on design of FSM evaluates the input as lexical correct or inccorect.
 * Saves the corresponding token type and if possible token content into 'token'.
 *
 * Returns NO_ERR if lexeme is lexically correct, LEX_ERR if lexeme is lexically incorrect
 * or INTERNAL_ERR if error occured with memory.
 */
error_code get_token(token* const token);

/**
 * Scans the input 'file' and based on design of FSM evaluates the input as lexical correct or inccorect.
 * Saves the corresponding token type and if possible token content into 'token'.
 *
 * Returns NO_ERR if lexeme is lexically correct, LEX_ERR if lexeme is lexically incorrect
 * or INTERNAL_ERR if error occured with memory.
 */
error_code get_token_file(token* const token, FILE* file);

#endif // IFJ_PROJEKT_2024_SCANNER_H

