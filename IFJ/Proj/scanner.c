// Project: Implementation of a compiler for the IFJ24 imperative language.
// Author: Radim Dvořák (xdvorar00)
//
// Implementation of scanner as deterministic FSM

#include "scanner.h"
#include "error.h"
#include "vector.h"
#include <assert.h>
#include <ctype.h>
#include <errno.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// String representation of the token type.
const char* const TOKEN_STRINGS[] = {
    // Tokens with contiguous indices
    "TOKEN_EOF",    // 0
    "TOKEN_IMPORT", // 1
    "TOKEN_AS",     // 2

    // Tokens starting at index 100
    [100] = "TOKEN_KW_CONST", // 100
    "TOKEN_KW_ELSE",          // 101
    "TOKEN_KW_FN",            // 102
    "TOKEN_KW_IF",            // 103
    "TOKEN_KW_INT",           // 104
    "TOKEN_KW_INT_NULL",      // 105
    "TOKEN_KW_FLOAT",         // 106
    "TOKEN_KW_FLOAT_NULL",    // 107
    "TOKEN_KW_NULL",          // 108
    "TOKEN_KW_PUB",           // 109
    "TOKEN_KW_RETURN",        // 110
    "TOKEN_KW_U8",            // 111
    "TOKEN_KW_SLICE",         // 112
    "TOKEN_KW_SLICE_NULL",    // 113
    "TOKEN_KW_VAR",           // 114
    "TOKEN_KW_VOID",          // 115
    "TOKEN_KW_WHILE",         // 116
    "TOKEN_KW_UNREACHABLE",   // 117
    "TOKEN_KW_FOR",           // 118
    "TOKEN_KW_BREAK",         // 119
    "TOKEN_KW_CONTINUE",      // 120
    "TOKEN_KW_BOOL",          // 121
    "TOKEN_KW_TRUE",          // 122
    "TOKEN_KW_FALSE",         // 123

    // Tokens starting at index 200
    [200] = "TOKEN_LIT_INT", // 200
    "TOKEN_LIT_FLOAT",       // 201
    "TOKEN_LIT_STRING",      // 202

    // Tokens starting at index 300
    [300] = "TOKEN_PLUS",   // 300
    "TOKEN_MINUS",          // 301
    "TOKEN_MUL",            // 302
    "TOKEN_DIV",            // 303
    "TOKEN_ORELSE",         // 304
    "TOKEN_IS_UNREACHABLE", // 305
    "TOKEN_NEG",            // 306
    "TOKEN_AND",            // 307
    "TOKEN_OR",             // 308

    // Relational operators starting at index 400
    [400] = "TOKEN_EQ",   // 400
    "TOKEN_NOT_EQ",       // 401
    "TOKEN_LESS_THAN",    // 402
    "TOKEN_GREATER_THAN", // 403
    "TOKEN_LESS_EQ",      // 404
    "TOKEN_GREATER_EQ",   // 405

    // Symbols starting at index 500
    [500] = "TOKEN_DOT", // 500
    "TOKEN_COMMA",       // 501
    "TOKEN_UNDESCORE",   // 502
    "TOKEN_ASSIGN",      // 503
    "TOKEN_COLON",       // 504
    "TOKEN_SEMICOLON",   // 505
    "TOKEN_VERT_BAR",    // 506
    "TOKEN_L_PAREN",     // 507
    "TOKEN_R_PAREN",     // 508
    "TOKEN_L_CURLY",     // 509
    "TOKEN_R_CURLY",     // 510

    // Identifier at index 1000
    [1000] = "TOKEN_IDENT" // 1000
};

typedef enum {
    START,                     // Starting state
    COMMENT_START,             // Token can be division or it can be comment.
    COMMENT,                   // "Eating" comment until new line.
    ZIG_KW,                    // Special keywords "@import", "@as"
    KEYWORD_OR_IDENT,          // Either keyword or identificator
    KEYWORD_SLICE,             // Start of token for slice.
    KEYWORD_SLICE_FINISH,      // Evaluation of string for slice.
    KEYWORD_NULL,              // Keywords starting with '?'
    KEYWORD_NULL_SLICE,        // Start of token for nullable slice.
    KEYWORD_NULL_SLICE_FINISH, // Evaluation of string for slice.
    IDENTIFIER,                // Token can be identifier.
    INTEGER_LIT_0,             // Token can be zero.
    INTEGER_LIT,               // Token can be integer.
    FLOAT_LIT_DOT,             // Token can be float literal.
    FLOAT_LIT_EXP,             // Got char for exponent 'e', 'E'.
    FLOAT_LIT_EXP_SIGN,        // Checking for potential sign after exponent.
    FLOAT_LIT_AFTER_DOT,       // Reading numbers after dot.
    FLOAT_LIT_AFTER_EXP,       // Reading numbers after exponent.
    STRING_LIT,                // Reading string literal.
    STRING_LIT_ESCAPE,         // Escape sequence in string literal.
    STRING_LIT_HEX_CHAR_1,     // Reading first digit from char typed by its hex value.
    STRING_LIT_HEX_CHAR_2,     // Reading second digit from char typed by its hex value.
    MULTI_LINE_STRING_LIT,          // Multi line string literals begin.
    MULTI_LINE_STRING_LIT_READ,     // Reading line of multiline string literal.
    MULTI_LINE_STRING_LIT_END_LINE, // "Eating" whitespaces until backslash.
    MULTI_LINE_STRING_LIT_NEW_LINE, // Puts character of new line.
} scanner_state;

/**
 * Sets token type of 'token' to 'type_to_assign'.
 *
 * Returns NO_ERROR.
 */
error_code set_token_type(token* const token, token_type type_to_assign) {
    assert(token != NULL);

    token->type = type_to_assign;

    return NO_ERR;
}

/**
 * Sets 'token' as integer literal and converts 'in_vec' as integer value into 'token' content.
 *
 * Returns NO_ERROR if conversion succeeds or INTERNAL_ERROR if error happens.
 */
error_code set_int_lit(token* const token, vector* in_vec) {
    assert(token != NULL);
    assert(in_vec != NULL);

    set_token_type(token, TOKEN_LIT_INT);

    char* string_convert = vec_to_str(&in_vec);

    char* endptr;
    token->content.int_lit = strtol(string_convert, &endptr, 10);
    free(string_convert);
    if (token->content.int_lit == 0 && errno != 0) {
        return INTERNAL_ERR;
    }

    return NO_ERR;
}

/**
 * Sets 'token' as float literal and converts 'in_vec' as floating point value into 'token' content.
 *
 * Returns NO_ERROR if conversion succeeds or INTERNAL_ERROR if error happens.
 */
error_code set_float_lit(token* const token, vector* in_vec) {
    assert(token != NULL);
    assert(in_vec != NULL);

    set_token_type(token, TOKEN_LIT_FLOAT);

    char* string_convert = vec_to_str(&in_vec);

    char* endptr;
    token->content.float_lit = strtod(string_convert, &endptr);
    free(string_convert);
    if (token->content.float_lit == 0.0F && errno != 0) {
        return INTERNAL_ERR;
    }

    return NO_ERR;
}

/**
 * Sets 'token' as string literal and assigns 'in_vec' as string into 'token' content.
 *
 * Returns NO_ERROR if assigment is succesful, otherwise INTERNAL_ERROR if error happens.
 */
error_code set_string_lit(token* const token, vector* in_vec) {
    assert(token != NULL);
    assert(in_vec != NULL);

    set_token_type(token, TOKEN_LIT_STRING);

    token->content.string_lit = vec_to_str(&in_vec);
    if (token->content.string_lit == NULL) {
        return INTERNAL_ERR;
    }

    return NO_ERR;
}

/**
 * Sets 'token' as identificator and assigns 'in_vec' as string into 'token' content.
 *
 * Returns NO_ERROR if assigment is succesful, otherwise INTERNAL_ERROR if error happens.
 */
error_code set_ident(token* const token, vector* in_vec) {
    assert(token != NULL);
    assert(in_vec != NULL);

    set_token_type(token, TOKEN_IDENT);

    token->content.id = vec_to_str(&in_vec);
    if (token->content.id == NULL) {
        return INTERNAL_ERR;
    }

    return NO_ERR;
}

/**
 * Converts char 'in', which is hexadecimal digit, into corresponding decimal value.
 *
 * Returns decimal value of char 'in'.
 */
int hex_to_dec(char in) {
    if (in >= 'A' && in <= 'F') {
        return in - 'A' + 10;
    } else if (in >= 'a' && in <= 'f') {
        return in - 'a' + 10;
    } else {
        return in - '0';
    }
}

/**
 * Gets two hexadecimal digits from top of 'in_vec' and converts it into corresponding char value.
 *
 * Returns value of character given by two hexadecimal digits.
 */
int convert_hex_char(vector* in_vec) {
    assert(in_vec != NULL);

    char* second_char = (char*)vec_pop(in_vec);
    char* first_char = (char*)vec_pop(in_vec);

    int first_digit = hex_to_dec(*first_char);
    int second_digit = hex_to_dec(*second_char);

    free(first_char);
    free(second_char);

    return 16 * first_digit + second_digit;
}

/**
 * Gets string from 'in_vec' and tries to match it for some known keyword then sets 'token' type as corresponding keyword,
 * otherwise it sets 'token' as identifier and saves its string value into 'token' content sets 'token' type as identifier.
 *
 * Return NO_ERROR if no error occured or INTERNAL_ERROR if conversion of string failed.
 */
error_code handle_keywords(token* const token, vector* in_vec){
    assert(token != NULL);
    assert(in_vec != NULL);

    char* cmp_string = vec_get_str(in_vec);
    if (!strcmp(cmp_string, "const")) {
        free(cmp_string);
        vec_free(&in_vec);
        return set_token_type(token, TOKEN_KW_CONST);
    } else if (!strcmp(cmp_string, "else")) {
        free(cmp_string);
        vec_free(&in_vec);
        return set_token_type(token, TOKEN_KW_ELSE);
    } else if (!strcmp(cmp_string, "fn")) {
        free(cmp_string);
        vec_free(&in_vec);
        return set_token_type(token, TOKEN_KW_FN);
    } else if (!strcmp(cmp_string, "if")) {
        free(cmp_string);
        vec_free(&in_vec);
        return set_token_type(token, TOKEN_KW_IF);
    } else if (!strcmp(cmp_string, "i32")) {
        free(cmp_string);
        vec_free(&in_vec);
        return set_token_type(token, TOKEN_KW_INT);
    } else if (!strcmp(cmp_string, "f64")) {
        free(cmp_string);
        vec_free(&in_vec);
        return set_token_type(token, TOKEN_KW_FLOAT);
    } else if (!strcmp(cmp_string, "null")) {
        free(cmp_string);
        vec_free(&in_vec);
        return set_token_type(token, TOKEN_KW_NULL);
    } else if (!strcmp(cmp_string, "pub")) {
        free(cmp_string);
        vec_free(&in_vec);
        return set_token_type(token, TOKEN_KW_PUB);
    } else if (!strcmp(cmp_string, "return")) {
        free(cmp_string);
        vec_free(&in_vec);
        return set_token_type(token, TOKEN_KW_RETURN);
    } else if (!strcmp(cmp_string, "u8")) {
        free(cmp_string);
        vec_free(&in_vec);
        return set_token_type(token, TOKEN_KW_U8);
    } else if (!strcmp(cmp_string, "var")) {
        free(cmp_string);
        vec_free(&in_vec);
        return set_token_type(token, TOKEN_KW_VAR);
    } else if (!strcmp(cmp_string, "void")) {
        free(cmp_string);
        vec_free(&in_vec);
        return set_token_type(token, TOKEN_KW_VOID);
    } else if (!strcmp(cmp_string, "while")) {
        free(cmp_string);
        vec_free(&in_vec);
        return set_token_type(token, TOKEN_KW_WHILE);
    } else if (!strcmp(cmp_string, "unreachable")) {
        free(cmp_string);
        vec_free(&in_vec);
        return set_token_type(token, TOKEN_KW_UNREACHABLE);
    } else if (!strcmp(cmp_string, "for")) {
        free(cmp_string);
        vec_free(&in_vec);
        return set_token_type(token, TOKEN_KW_FOR);
    } else if (!strcmp(cmp_string, "break")) {
        free(cmp_string);
        vec_free(&in_vec);
        return set_token_type(token, TOKEN_KW_BREAK);
    } else if (!strcmp(cmp_string, "continue")) {
        free(cmp_string);
        vec_free(&in_vec);
        return set_token_type(token, TOKEN_KW_CONTINUE);
    } else if (!strcmp(cmp_string, "bool")) {
        free(cmp_string);
        vec_free(&in_vec);
        return set_token_type(token, TOKEN_KW_BOOL);
    } else if (!strcmp(cmp_string, "true")) {
        free(cmp_string);
        vec_free(&in_vec);
        return set_token_type(token, TOKEN_KW_TRUE);
    } else if (!strcmp(cmp_string, "false")) {
        free(cmp_string);
        vec_free(&in_vec);
        return set_token_type(token, TOKEN_KW_FALSE);
    }
    // Although "orelse" is operator, it is made from letters, so i
    // am treating it as keyword
    else if (!strcmp(cmp_string, "orelse")) {
        free(cmp_string);
        vec_free(&in_vec);
        return set_token_type(token, TOKEN_ORELSE);
    } else if (!strcmp(cmp_string, "and")) {
        free(cmp_string);
        vec_free(&in_vec);
        return set_token_type(token, TOKEN_AND);
    } else if (!strcmp(cmp_string, "or")) {
        free(cmp_string);
        vec_free(&in_vec);
        return set_token_type(token, TOKEN_OR);
    } else {
        free(cmp_string);
        return set_ident(token,in_vec);
    }
}

error_code get_token(token* const token) {
    return get_token_file(token, stdin);
}

error_code get_token_file(token* const token, FILE* file){
    assert(token != NULL);
    assert(file != NULL);

    vector* buffer;
    int c;

    scanner_state state = START;

    while (1) {
        c = getc(file);
        switch (state) {
        // Initial state when starting reading input lexeme
        case START:
            switch (c) {
            case '\\':
                buffer = vec_init(20,sizeof(char));
                state = MULTI_LINE_STRING_LIT;
                break;
            // Token for 'end of file'
            case EOF:
                return set_token_type(token, TOKEN_EOF);
            // Binary operators
            case '+':
                return set_token_type(token, TOKEN_PLUS);
            case '-':
                return set_token_type(token, TOKEN_MINUS);
            case '*':
                return set_token_type(token, TOKEN_MUL);
            // Either start of comment or token for 'division'
            case '/':
                state = COMMENT_START;
                break;
            // Token for 'assigment' or 'equal'
            case '=':
                c = getc(file);
                if (c == '=') {
                    return set_token_type(token, TOKEN_EQ);
                } else {
                    ungetc(c, file);
                    return set_token_type(token, TOKEN_ASSIGN);
                }
            // Token for '.' or unary operator '.?', which is equvivalent to "orelse unreachable".
            case '.':
                c = getc(file);
                if (c == '?') {
                    return set_token_type(token, TOKEN_IS_UNREACHABLE);
                } else {
                    ungetc(c, file);
                    return set_token_type(token, TOKEN_DOT);
                }
            case ',':
                return set_token_type(token, TOKEN_COMMA);
            // Token for operator 'negation' or operator 'not equal'
            case '!':
                c = getc(file);
                if (c == '=') {
                    return set_token_type(token, TOKEN_NOT_EQ);
                } else {
                    ungetc(c, file);
                    return set_token_type(token, TOKEN_NEG);
                }
            // Lexeme starts with char '?', it can be optionable data type ('?i32', '?f64', '?[]u8').
            case '?':
                buffer = vec_init(20,sizeof(char));
                vec_push(buffer, &c);
                state = KEYWORD_NULL;
                break;
            // Lexeme starts with char '?', it can be slice data type '[]u8'.
            case '[':
                buffer = vec_init(20,sizeof(char));
                vec_push(buffer, &c);
                state = KEYWORD_SLICE;
                break;
            // Lexeme starts with char '?', it an be keywords for compatibility with Zig ('@as', '@import').
            case '@':
                buffer = vec_init(20,sizeof(char));
                vec_push(buffer, &c);
                state = ZIG_KW;
                break;
            // Token for relation operator 'less' or relation operator 'less equal'
            case '<':
                c = getc(file);
                if (c == '=') {
                    return set_token_type(token, TOKEN_LESS_EQ);
                } else {
                    ungetc(c, file);
                    return set_token_type(token, TOKEN_LESS_THAN);
                }
            // Token for relation operator 'more' or relation operator 'more equal'
            case '>':
                c = getc(file);
                if (c == '=') {
                    return set_token_type(token, TOKEN_GREATER_EQ);
                } else {
                    ungetc(c, file);
                    return set_token_type(token, TOKEN_GREATER_THAN);
                }
            // Single character token, mostly symobols
            case ':':
                return set_token_type(token, TOKEN_COLON);
            case ';':
                return set_token_type(token, TOKEN_SEMICOLON);
            case '|':
                return set_token_type(token, TOKEN_VERT_BAR);
            case '(':
                return set_token_type(token, TOKEN_L_PAREN);
            case ')':
                return set_token_type(token, TOKEN_R_PAREN);
            case '{':
                return set_token_type(token, TOKEN_L_CURLY);
            case '}':
                return set_token_type(token, TOKEN_R_CURLY);
            // Start of string literal
            case '\"':
                buffer = vec_init(20,sizeof(char));
                state = STRING_LIT;
                break;
            // Token for throwaway assigment '_' or identifier starting with undescore.
            case '_':
                buffer = vec_init(20,sizeof(char));
                vec_push(buffer, &c);
                c = getc(file);
                if (isalnum(c) || c == '_') {
                    vec_push(buffer, &c);
                    state = IDENTIFIER;
                    break;
                } else {
                    vec_free(&buffer);
                    return set_token_type(token, TOKEN_UNDESCORE);
                }
            default:
                // First character is lowercase letter, it can be keyword or identifier.
                if (c >= 'a' && c <= 'z') {
                    buffer = vec_init(20,sizeof(char));
                    vec_push(buffer, &c);
                    state = KEYWORD_OR_IDENT;
                    break;
                // First character is uperrcase letter, it can be an identifier.
                } else if (c >= 'A' && c <= 'Z') {
                    buffer = vec_init(20,sizeof(char));
                    vec_push(buffer, &c);
                    state = IDENTIFIER;
                    break;
                // First character is number zero, it can be either lone zero or float literal.
                } else if (c == '0') {
                    buffer = vec_init(20,sizeof(char));
                    vec_push(buffer, &c);
                    state = INTEGER_LIT_0;
                    break;
                // First character are numbers except zero, it can be either integer literal or float literal.
                } else if (c > '0' && c <= '9') {
                    buffer = vec_init(20,sizeof(char));
                    vec_push(buffer, &c);
                    state = INTEGER_LIT;
                    break;
                // Skips all whitespace characters.
                } else if (isspace(c)){
                    break;
                // Other character causes lexical error.
                } else {
                    return LEXICAL_ERR;
                }
            }
            break;
        // Checks next character for another '/', if it is there it's comment,
        // otherwise token for 'division'
        case COMMENT_START:
            if (c == '/') {
                state = COMMENT;
                break;
            } else {
                return set_token_type(token, TOKEN_DIV);
            }
            break;
        // "Eats" all characters until char for new line '\n' or 'EOF'
        case COMMENT:
            if (c == '\n') {
                state = START;
                break;
            } else if(c == EOF) {
                return set_token_type(token,TOKEN_EOF);
            } else {
                break;
            }
        // Special Zig keywords ('@as', @import)
        case ZIG_KW:
            if (c >= 'a' && c <= 'z') {
                vec_push(buffer, &c);
                break;
            }
            ungetc(c, file);
            char* cmp_string = vec_to_str(&buffer);

            if (!strcmp(cmp_string, "@as")) {
                free(cmp_string);
                return set_token_type(token, TOKEN_AS);
            }
            if (!strcmp(cmp_string, "@import")) {
                free(cmp_string);
                return set_token_type(token, TOKEN_IMPORT);
            } else {
                free(cmp_string);
                return LEXICAL_ERR;
            }
        // Pushes all lowercase letters and numbers into buffer, when uppercase letter
        // or undescore is found, it gets into state for identifiers, otherwise it compares with known keywords.
        case KEYWORD_OR_IDENT:
            if ((c >= 'a' && c <= 'z') || (c >= '0' && c <= '9')) {
                vec_push(buffer, &c);
                break;
            } else if ((c >= 'A' && c <= 'Z') || c == '_') {
                vec_push(buffer, &c);
                state = IDENTIFIER;
                break;
            } else {
                ungetc(c, file);
                return handle_keywords(token,buffer);
            }
        // Token is either slice, or invokes lexical error.
        case KEYWORD_SLICE:
            if (c == ']') {
                vec_push(buffer, &c);
                state = KEYWORD_SLICE_FINISH;
                break;
            } else {
                vec_free(&buffer);
                return LEXICAL_ERR;
            }
        // Pushes all lowercase letters and numbers into buffer, otherwise it compares with identifier for slice ('[]u8').
        case KEYWORD_SLICE_FINISH:
            if ((c >= 'a' && c <= 'z') || (c >= '0' && c <= '9')) {
                vec_push(buffer, &c);
                break;
            } else {
                ungetc(c, file);

                char* cmp_string = vec_to_str(&buffer);
                if (!strcmp(cmp_string, "[]u8")) {
                    free(cmp_string);
                    return set_token_type(token, TOKEN_KW_SLICE);
                } else {
                    free(cmp_string);
                    return LEXICAL_ERR;
                }
            }
        // Pushes all lowercase letters and numbers into buffer, when left bracket
        // is found, it gets into state for optionable slice, otherwise it compares with '?i32' and '?f64'.
        case KEYWORD_NULL:
            if ((c >= 'a' && c <= 'z') || (c >= '0' && c <= '9')) {
                vec_push(buffer, &c);
                break;
            } else if (c == '[') {
                vec_push(buffer, &c);
                state = KEYWORD_NULL_SLICE;
                break;
            } else {
                ungetc(c, file);

                char* cmp_string = vec_to_str(&buffer);
                if (!strcmp(cmp_string, "?i32")) {
                    free(cmp_string);
                    return set_token_type(token, TOKEN_KW_INT_NULL);
                } else if (!strcmp(cmp_string, "?f64")) {
                    free(cmp_string);
                    return set_token_type(token, TOKEN_KW_FLOAT_NULL);
                } else {
                    free(cmp_string);
                    return LEXICAL_ERR;
                }
            }
        // When right bracket is found,
        case KEYWORD_NULL_SLICE:
            if (c == ']') {
                vec_push(buffer, &c);
                state = KEYWORD_NULL_SLICE_FINISH;
                break;
            } else {
                vec_free(&buffer);
                return LEXICAL_ERR;
            }
        // Pushes all lowercase letters and numbers into buffer until it gets some other character,
        // then it compares with '?[]u8'.
        case KEYWORD_NULL_SLICE_FINISH:
            if ((c >= 'a' && c <= 'z') || (c >= '0' && c <= '9')) {
                vec_push(buffer, &c);
                break;
            } else {
                ungetc(c, file);

                char* cmp_string = vec_to_str(&buffer);
                if (!strcmp(cmp_string, "?[]u8")) {
                    free(cmp_string);
                    return set_token_type(token, TOKEN_KW_SLICE_NULL);
                } else {
                    free(cmp_string);
                    return LEXICAL_ERR;
                }
            }
        // Pushes all letters, digits and undescores until it reads another character.
        case IDENTIFIER:
            if (isalnum(c) || c == '_') {
                vec_push(buffer, &c);
                break;
            } else {
                ungetc(c, file);
                return set_ident(token, buffer);
            }
        // Checks if next character is digit, if yes then invokes lexical error,
        // otherwise returns integer literal with value zero
        case INTEGER_LIT_0:
            if (isdigit(c)) {
                vec_free(&buffer);
                return LEXICAL_ERR;
            } else if (c == '.') {
                vec_push(buffer, &c);
                state = FLOAT_LIT_DOT;
                break;
            } else if (c == 'e' || c == 'E') {
                vec_push(buffer, &c);
                state = FLOAT_LIT_EXP;
                break;
            } else {
                ungetc(c, file);
                return set_int_lit(token, buffer);
            }
        // Checks if character is digit, stays in the same state
        // or if its either '.' or 'E','e', then it gets into float literal states.
        // If some other character is found returns integer literal.
        case INTEGER_LIT:
            if (isdigit(c)) {
                vec_push(buffer, &c);
                break;
            } else if (c == '.') {
                vec_push(buffer, &c);
                state = FLOAT_LIT_DOT;
                break;
            } else if (c == 'E' || c == 'e') {
                vec_push(buffer, &c);
                state = FLOAT_LIT_EXP;
                break;
            } else {
                ungetc(c, file);
                return set_int_lit(token, buffer);
            }
        // Previously 'e' or 'E' was found and this state ensures that it's followed by at least one digit.
        case FLOAT_LIT_DOT:
            if (isdigit(c)) {
                vec_push(buffer, &c);
                state = FLOAT_LIT_AFTER_DOT;
                break;
            } else {
                vec_free(&buffer);
                return LEXICAL_ERR;
            }
        // Checks if exponent is followed by sign of exponent ('-','+')
        // or by digit, otherwise invokes lexical error.
        case FLOAT_LIT_EXP:
            vec_push(buffer, &c);
            if (isdigit(c)) {
                state = FLOAT_LIT_AFTER_EXP;
                break;
            } else if (c == '+' || c == '-') {
                state = FLOAT_LIT_EXP_SIGN;
                break;
            } else {
                vec_free(&buffer);
                return LEXICAL_ERR;
            }
        // Previously '-' or '+' was found and this state ensures that it's followed by at least one digit.
        case FLOAT_LIT_EXP_SIGN:
            if (isdigit(c)) {
                vec_push(buffer, &c);
                state = FLOAT_LIT_AFTER_EXP;
                break;
            } else {
                vec_free(&buffer);
                return LEXICAL_ERR;
            }
        // Previously '.' was found and this state ensures that it's followed by at least one digit.
        case FLOAT_LIT_AFTER_DOT:
            if (isdigit(c)) {
                vec_push(buffer, &c);
                break;
            } else if (c == 'e' || c == 'E') {
                vec_push(buffer, &c);
                state = FLOAT_LIT_EXP;
                break;
            } else {
                ungetc(c, file);
                return set_float_lit(token, buffer);
            }
        // Pushes all consecutive digits into buffer.
        case FLOAT_LIT_AFTER_EXP:
            if (isdigit(c)) {
                vec_push(buffer, &c);
                break;
            } else {
                ungetc(c, file);
                return set_float_lit(token, buffer);
            }
        // Pushes all consecutive characters into buffer,
        // until '\"' is found, then saves content of buffer into token,
        // or resolves escape sequences.
        case STRING_LIT:
            if (c == '\\') {
                state = STRING_LIT_ESCAPE;
                break;
            } else if (c == EOF){
                ungetc(c,file);
                return set_string_lit(token, buffer);
            } else if (c == '\"') {
                return set_string_lit(token, buffer);
            } else if (c == '\n') {
                vec_free(&buffer);
                return LEXICAL_ERR;
            } else {
                vec_push(buffer, &c);
                break;
            }
        // Checks if the given escape sequence is valid and pushes into buffer corresponding character.
        // Then it continues in reading of string literal.
        case STRING_LIT_ESCAPE:
            if (c == '"') {
                vec_pushchar(buffer, '\"');
                state = STRING_LIT;
                break;
            } else if (c == 'n') {
                vec_pushchar(buffer, '\n');
                state = STRING_LIT;
                break;
            } else if (c == 'r') {
                vec_pushchar(buffer, '\r');
                state = STRING_LIT;
                break;
            } else if (c == 't') {
                vec_pushchar(buffer, '\t');
                state = STRING_LIT;
                break;
            } else if (c == '\\') {
                vec_pushchar(buffer, '\\');
                state = STRING_LIT;
                break;
            } else if (c == 'x') {
                state = STRING_LIT_HEX_CHAR_1;
                break;
            } else {
                vec_free(&buffer);
                return LEXICAL_ERR;
            }
        // Saves first valid hexadecimal number into buffer, to be later resolved as single character.
        case STRING_LIT_HEX_CHAR_1:
            if (isxdigit(c)) {
                vec_push(buffer, &c);
                state = STRING_LIT_HEX_CHAR_2;
                break;
            } else {
                vec_free(&buffer);
                return LEXICAL_ERR;
            }
        // Saves second valid hexadecimal number into buffer, and pushes character into buffer with value given
        // by two hexadecimal digits.
        case STRING_LIT_HEX_CHAR_2:
            if (isxdigit(c)) {
                vec_push(buffer, &c);

                int result_number = convert_hex_char(buffer);
                vec_push(buffer, &result_number);

                state = STRING_LIT;
                break;
            } else {
                vec_free(&buffer);
                return LEXICAL_ERR;
            }
        // Checks if there are two consectutive characters '\\'.
        case MULTI_LINE_STRING_LIT:
            if (c == '\\') {
                state = MULTI_LINE_STRING_LIT_READ;
                break;
            } else {
                vec_free(&buffer);
                return LEXICAL_ERR;
            }
        // Pushes all consecutive characters into buffer, until '\n' is found,
        // then it checks if multiline string literal continues on next line.
        case MULTI_LINE_STRING_LIT_READ:
            if (c == '\n') {
                state = MULTI_LINE_STRING_LIT_END_LINE;
                break;
            } else if (c == EOF){
                ungetc(c,file);
                return set_string_lit(token, buffer);
            } else {
                vec_push(buffer, &c);
                break;
            }
        // "Eats" all whitespaces on new line, and looks for character '\\',
        // otherwise returns multiline string literal.
        case MULTI_LINE_STRING_LIT_END_LINE:
            if (isblank(c)) {
                break;
            } else if (c == '\\') {
                state = MULTI_LINE_STRING_LIT_NEW_LINE;
                break;
            } else {
                ungetc(c, file);
                return set_string_lit(token, buffer);
            }
        // On next line two consectutive characters '\\' were found, pushes into buffer character '\n'
        // and continues reading string literal.
        case MULTI_LINE_STRING_LIT_NEW_LINE:
            if (c == '\\') {
                vec_pushchar(buffer, '\n');
                state = MULTI_LINE_STRING_LIT_READ;
                break;
            } else {
                ungetc(c, file);
                return set_string_lit(token, buffer);
            }
        default:
            vec_free(&buffer);
            return LEXICAL_ERR;
        }
    }
}
