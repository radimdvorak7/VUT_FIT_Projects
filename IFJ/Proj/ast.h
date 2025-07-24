// Projekt: Implementace překladače imperativního jazyka IFJ23
// Autoři: Marek Slaný (xslany03), František Sedláček ( xsedla2d )
//
// Abstract syntax tree implementation.

#ifndef IFJ_AST_H
#define IFJ_AST_H

#include "data_types.h"
#include <stdbool.h>
#include <stdint.h>
#define INITIAL_CHILDREN_CAP 2

// Definition of node types
typedef enum {
    ERR = -1,
    PROG = 0,   ///< Root node
    VAR_DEF,    ///< Variable definition
    CONST_DEF,  ///< Constant definition
    VAR,        ///< Variable
    LITERAL,    ///< Literal
    NULL_LIT,   ///< Null as a literal
    FUNC,       ///< Function
    STMT_LIST,  ///< Statement block
    COND,       ///< Condition expression
    NULL_COND,  ///< Condition
    WHILE,      ///< While loop
    WHILE_CNT,  ///< While loop continue block
    WHILE_ELSE, ///< While loop ELSE part
    FOR,        ///< For loop
    IF,         ///< Condition branching
    ELSE,       ///< Else part of a conditional block
    EXPR,       ///< Expression
    OPT_EXPR, ///< optional expression (might not have an expression child node)
    ASSIGN,   ///< Assignment to a variable
    ASSIGN_THROW_AWAY, ///< Assigment to void. ( '_' character )
    FUNC_DEF,          ///< Function definition
    PARAM_LIST,        ///< Definition of parameters
    PARAM,
    RETURN,           ///< Return statement
    TYPE,             ///< Type definition
    INPUT_PARAM_LIST, ///< Function call passed parameter list
    INPUT_PARAM,      ///< Function call passed parameter
    CONTINUE,         ///< Continue statement
    BREAK,            ///< Break statement
    IF_TERNARY,       ///< Ternary operator
    // expression part, ordered by precedence
    IS_UNREACHABLE, ///< IS NULL unary postfix operator '.?'
    MUL,            ///< Multiplication
    DIV,            ///< Division
    ADD,            ///< Addition
    SUB,            ///< Subtraction
    ORELSE,         ///< OR ELSE operator
    INT_NEGATE,
    EQUAL,     ///< Comparison
    NOT_EQUAL, ///< Negative comparison
    EQUALMORE, ///< Greater than or equal to
    EQUALLESS, ///< Lesser than or equal to
    MORE,      ///< Bigger than
    LESS,      ///< Lesser than
    B_AND,     ///< AND binary operator
    B_OR,      ///< OR binary operator
    B_NEG,
    UNREACHABLE,
    AS_FUNC

} node_type;

extern const char* const NODE_TYPE_STRINGS[];

// Information about the node stored in AST.
typedef struct {
    node_type type;
    data_type ret_value; // return type of this node
    bool global;
    char* scope_id;
    union {
        int32_t int_value;
        double float_value;
        char* str_value;
        bool bool_value;
        char* id_name;
        char* label;
    }; // Union for storing values of literals, set to NULL if unused
} node_data;

typedef struct n {
    node_data data;
    struct n* ancestor;
    int child_cap;
    int child_count;
    struct n** children;
} node;

void print_children(node* n);

node* create_node(node_type type);
bool append_node(node* child, node* ancestor);
node* get_root(node* n);
void delete_tree(node** nod);
node* get_child_by_type(node* n, node_type type, int order);
node* get_ancestor_by_type(node* n, node_type type, int order);
bool has_parent_type(node* n, node_type type);
void print_tree(node* n);
const char* get_type_str(int i);
#endif
