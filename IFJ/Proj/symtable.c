// Projekt: Implementace překladače imperativního jazyka IFJ24
// Autor: Dominik Václavík (xvacla37)

#include "symtable.h"
#include "ast.h"

#include <stdlib.h>
#include <string.h>

#include <assert.h>
#include <stdio.h>

void rb_tree_init(b_tree* tree) {
    assert(tree != NULL);

    tree->root = NULL;
}

b_node* rb_tree_alloc_node(b_node* parent, char* symbol, b_tree_data data) {
    assert(symbol != NULL);

    b_node* node = malloc(sizeof(b_node));
    if (node == NULL)
        return NULL;
    node->parent = parent;
    node->left = NULL;
    node->right = NULL;
    node->color = RED;
    node->symbol = symbol;
    node->data = data;
    return node;
}

void left_rotate(b_node* x, b_tree* tree) {
    b_node* y = x->right;
    x->right = y->left;
    if (y->left != NULL)
        y->left->parent = x;

    y->parent = x->parent;

    if (x->parent == NULL)
        tree->root = y;
    else if (x == x->parent->left)
        x->parent->left = y;
    else
        x->parent->right = y;

    y->left = x;
    x->parent = y;
}

void right_rotate(b_node* x, b_tree* tree) {
    b_node* y = x->left;
    x->left = y->right;
    if (y->right != NULL)
        y->right->parent = x;

    y->parent = x->parent;

    if (x->parent == NULL)
        tree->root = y;
    else if (x == x->parent->left)
        x->parent->left = y;
    else
        x->parent->right = y;

    y->right = x;
    x->parent = y;
}

void rb_tree_fix_insert(b_node* node, b_tree* tree) {
    b_node* parent = NULL;
    b_node* grandparent = NULL;

    while ((node != tree->root) && (node->color != BLACK) &&
           (node->parent->color == RED)) {
        parent = node->parent;
        grandparent = parent->parent;

        // Parent is the left child of grandparent
        if (parent == grandparent->left) {
            b_node* uncle = grandparent->right;

            // Case 1: Uncle is red, recoloring needed
            if (uncle != NULL && uncle->color == RED) {
                grandparent->color = RED;
                parent->color = BLACK;
                uncle->color = BLACK;
                node = grandparent;
            } else {
                // Case 2: Node is right child of its parent, left rotate needed
                if (node == parent->right) {
                    left_rotate(parent, tree);
                    node = parent;
                    parent = node->parent;
                }

                // Case 3: Node is left child of its parent, right rotate needed
                right_rotate(grandparent, tree);
                parent->color = BLACK;
                grandparent->color = RED;
                node = parent;
            }
        }
        // Parent is the right child of grandparent
        else {
            b_node* uncle = grandparent->left;

            // Case 1: Uncle is red, recoloring needed
            if (uncle != NULL && uncle->color == RED) {
                grandparent->color = RED;
                parent->color = BLACK;
                uncle->color = BLACK;
                node = grandparent;
            } else {
                // Case 2: Node is left child of its parent, right rotate needed
                if (node == parent->left) {
                    right_rotate(parent, tree);
                    node = parent;
                    parent = node->parent;
                }

                // Case 3: Node is right child of its parent, left rotate needed
                left_rotate(grandparent, tree);
                parent->color = BLACK;
                grandparent->color = RED;
                node = parent;
            }
        }
    }

    // Ensure the root is always black
    tree->root->color = BLACK;
}

int rb_tree_insert(b_tree* tree, char* symbol, b_tree_data data) {
    assert(tree != NULL);
    assert(symbol != NULL);

    b_node* current = tree->root;
    b_node* parent = NULL;

    int cmp_res;

    while (current != NULL) {
        parent = current;
        cmp_res = strcmp(current->symbol, symbol);
        if (cmp_res < 0) {
            current = current->right;
        } else if (cmp_res > 0) {
            current = current->left;
        } else {
            free(symbol);
            fprintf(stderr, "Symbol already in table!\n");
            return INSERT_ALREADY_IN;
        }
    }

    current = rb_tree_alloc_node(parent, symbol, data);
    if (current == NULL) {
        free(symbol);
        return INSERT_MEM_ERR;
    }

    if (parent == NULL) {
        // current->color = BLACK;
        tree->root = current;
        // return INSERT_SUCCESS;
    }

    else if (cmp_res < 0) {
        parent->right = current;
    } else {
        parent->left = current;
    }

    // TODO: balance tree if it is red
    rb_tree_fix_insert(current, tree);

    return INSERT_SUCCESS;
}

b_tree_data* rb_tree_find(b_tree* tree, const char* symbol) {
    assert(tree != NULL);
    assert(symbol != NULL);

    b_node** node = &(tree->root);
    while ((*node) != NULL) {
        int cmp_res = strcmp((*node)->symbol, symbol);
        if (cmp_res < 0) {
            node = &((*node)->right);
        } else if (cmp_res > 0) {
            node = &((*node)->left);
        } else {
            return &(*node)->data;
        }
    }

    return NULL;
}

void rb_free_node_data(b_node* node) {
    // If scope_id is NULL nothing happens
    // otherwise it frees the pointer.
    if (node->data.symbol_type == SYM_VAR ||
        node->data.symbol_type == SYM_CONST) {
        free(node->data.scope_id);
        node->data.scope_id = NULL;
    }
    if (node->data.symbol_type == SYM_FUNC) {
        vec_free(&node->data.func_parameters);
    }
}

void rb_tree_free(b_tree* tree) {
    assert(tree != NULL);

    b_node* curr = tree->root;
    while (curr != NULL) {
        if (curr->left != NULL) {
            curr = curr->left;
            curr->parent->left = NULL;
            continue;
        }
        if (curr->right != NULL) {
            curr = curr->right;
            curr->parent->right = NULL;
            continue;
        }
        b_node* parent = curr->parent;
        free(curr->symbol);
        rb_free_node_data(curr);
        free(curr);
        curr = parent;
    }
    tree->root = NULL;
}

const char* get_node_name(b_node* node) {
    if (node == NULL)
        return "NULL";
    return node->symbol;
}

void rb_tree_print_rec(b_node* node, int depth) {
    if (node == NULL)
        return;
    printf("Node: %s\nColor: %s\nDepth: %d\n\tParent: %s\n\tLeft: %s\n\tRight: "
           "%s\n\n",
           get_node_name(node), node->color == RED ? "RED" : "BLACK", depth,
           get_node_name(node->parent), get_node_name(node->left),
           get_node_name(node->right));
    depth++;
    rb_tree_print_rec(node->left, depth);
    rb_tree_print_rec(node->right, depth);
}

void rb_tree_print(b_tree* tree) { rb_tree_print_rec(tree->root, 0); }
