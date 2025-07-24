// Projekt: Implementace překladače imperativního jazyka IFJ24
// Autor: Marek Slaný   (xslany03)

#include "ast.h"
#include "data_types.h"
#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

// Allocates memory for struct members and initializes them with default
// values, type of the allocated node is specified by the argument.
// Returns pointer to the created node or NULL if allocation fails.
node* create_node(node_type type) {
    node* n = malloc(sizeof(node));
    if (n == NULL) {
        fprintf(stderr, "Failed to malloc memory for node\n");
        return NULL;
    }
    node_data data = {0};
    data.type = type;
    data.ret_value = UNKNOWN;
    data.global = false;
    data.scope_id = 0;
    data.str_value = NULL;
    n->data = data;
    n->ancestor = NULL;
    n->child_cap = INITIAL_CHILDREN_CAP;
    n->child_count = 0;
    n->children = calloc(n->child_cap, sizeof(node*));
    if (n->children == NULL) {
        free(n);
        fprintf(stderr, "Failed to malloc memory for node children\n");
        return NULL;
    }
    return n;
}

const char* const NODE_TYPE_STRINGS[] = {"PROG",
                                         "VAR_DEF",
                                         "CONST_DEF",
                                         "VAR",
                                         "LITERAL",
                                         "NULL_LIT",
                                         "FUNC",
                                         "STMT_LIST",
                                         "COND",
                                         "NULL_COND",
                                         "WHILE",
                                         "WHILE_CNT",
                                         "WHILE_ELSE",
                                         "FOR",
                                         "IF",
                                         "ELSE",
                                         "EXPR",
                                         "OPT_EXPR",
                                         "ASSIGN",
                                         "ASSIGN_THROW_AWAY",
                                         "FUNC_DEF",
                                         "PARAM_LIST",
                                         "PARAM",
                                         "RETURN",
                                         "TYPE",
                                         "INPUT_PARAM_LIST",
                                         "INPUT_PARAM",
                                         "CONTINUE",
                                         "BREAK",
                                         "IF_TERNARY",
                                         "IS_UNREACHABLE",
                                         "MUL",
                                         "DIV",
                                         "ADD",
                                         "SUB",
                                         "ORELSE",
                                         "INT_NEGATE",
                                         "EQUAL",
                                         "NOT_EQUAL",
                                         "EQUALMORE",
                                         "EQUALLESS",
                                         "MORE",
                                         "LESS",
                                         "B_AND",
                                         "B_OR",
                                         "B_NEG",
                                         "UNREACHABLE",
                                         "AS_FUNC"};

// Prints the children of the node
void print_children(node* n) {
    fprintf(stderr, "%s\n", DATA_TYPE_STRINGS[n->data.ret_value]);
    for (int i = 0; i < n->child_count; i++) {
        fprintf(stderr, "\t|\n");
        fprintf(stderr, "\t`-- %s\n", DATA_TYPE_STRINGS[n->data.ret_value]);
    }
}

void print_tree(node* n) {
    fprintf(stderr, "NODE: %s", NODE_TYPE_STRINGS[n->data.type]);
    if (n->ancestor != NULL) {
        fprintf(stderr, " => %s", NODE_TYPE_STRINGS[n->ancestor->data.type]);
    }
    fprintf(stderr, "\n");
    if (n->data.scope_id != NULL) {
        fprintf(stderr, "\033[34mScope ID: %s\033[0m\n", n->data.scope_id);
    }
    for (int i = 0; i < n->child_count; i++) {
        fprintf(stderr, "%p\n", n->children[i]);
        print_tree(n->children[i]);
    }
}

// Appends 'child' under the 'ancestor'
bool append_node(node* child, node* ancestor) {
    // Cannot append 'NULL'
    assert(child != NULL);

    // Child is already connected
    if (child->ancestor != NULL) {
        return false;
    }
    if (ancestor->child_count >= ancestor->child_cap) {
        // Increase the size for children.
        node** new_ptr = realloc(ancestor->children,
                                 ancestor->child_cap * 2 * sizeof(node*));
        // Failed to allocate the memory
        if (new_ptr == NULL) {
            fprintf(stderr, "Failed to increase space for children\n");
            return false;
        }
        ancestor->child_cap *= 2;
        ancestor->children = new_ptr;
    }
    ancestor->children[ancestor->child_count] = child;
    ancestor->child_count++;
    child->ancestor = ancestor;

    return true;
}

node* get_root(node* n) {
    while (n->ancestor != NULL) {
        n = n->ancestor;
    }
    return n;
}

// Gets the child of node 'n' with type 'type', 'order' is the index of
// the child (with the desired type) to return. Returns a pointer to the
// child if found, otherwise returns NULL
node* get_child_by_type(node* n, node_type type, int order) {
    if (n == NULL) {
        return NULL;
    }

    int count = 0;
    for (int i = 0; i < n->child_count; i++) {
        if (n->children[i]->data.type == type) {
            if (count == order) {
                return n->children[i];
            }
            count++;
        }
    }
    return NULL;
}

// Goes through the ancestors of node 'n' to find a node of type 'type',
// order specifies the index of the node.
// Returns pointer to the node if found, otherwise NULL
node* get_ancestor_by_type(node* n, node_type type, int order) {
    if (n == NULL) {
        return NULL;
    }

    int count = 0;
    while (n->ancestor != NULL) {
        n = n->ancestor;
        if (n->data.type == type) {
            if (order == count) {
                return n;
            }
            count++;
        }
    }
    return NULL;
}

void free_data(node* n) {
    // Free inner data by type
    switch (n->data.type) {
    case LITERAL:
        if (n->data.ret_value == U8_SLICE) {
            free(n->data.str_value);
            n->data.str_value = NULL;
        }
        break;
    case FUNC:
    case FUNC_DEF:
    case VAR_DEF:
    case CONST_DEF:
    case PARAM:
    case ASSIGN:
    case VAR:
        free(n->data.id_name);
        n->data.id_name = NULL;
        break;
    case WHILE:
        if (n->data.label != NULL) {
            free(n->data.label);
            n->data.label = NULL;
        }
        break;
    default:
        break;
    }
}

// Frees the whole subtree of 'nod' (including 'nod')
void delete_tree(node** nod) {
    node* n = *nod;
    if (n == NULL) {
        return;
    }
    free_data(n);
    for (int i = 0; i < n->child_count; i++) {
        delete_tree(&(n->children[i]));
    }
    free(n->children);
    free(n);
    *nod = NULL;
}

bool has_parent_type(node* n, node_type type) {
    n = n->ancestor;
    while (n != NULL) {
        if (n->data.type == type)
            return true;
        n = n->ancestor;
    }
    return false;
}

const char* get_type_str(int i) { return NODE_TYPE_STRINGS[i]; }
