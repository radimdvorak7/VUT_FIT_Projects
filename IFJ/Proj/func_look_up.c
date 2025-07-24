// Projekt: Implementace přeladače jazyka IFJ24
// Autor: Dominik Václavík (xvacla37)

#include "func_look_up.h"

#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <stdbool.h>

void fl_tree_init(fl_tree* tree) {
    assert(tree != NULL);

    tree->root = NULL;
}

fl_node* fl_tree_alloc_node(fl_node* parent, char* symbol, char* translated, char* code) {
    assert(symbol != NULL);

    fl_node* node = malloc(sizeof(fl_node));
    if (node == NULL)
        return NULL;
    node->parent = parent;
    node->left = NULL;
    node->right = NULL;
    node->color = RED;
    node->symbol = symbol;
    node->data.translated = translated;
    node->data.code = code;
    node->data.counter = 0;
    return node;
}

void fl_left_rotate(fl_node* x, fl_tree* tree) {
    fl_node* y = x->right;
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

void fl_right_rotate(fl_node* x, fl_tree* tree) {
    fl_node* y = x->left;
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

void fl_tree_fix_insert(fl_node* node, fl_tree* tree) {
    fl_node* parent = NULL;
    fl_node* grandparent = NULL;

    while ((node != tree->root) && (node->color != BLACK) &&
           (node->parent->color == RED)) {
        parent = node->parent;
        grandparent = parent->parent;

        // Parent is the left child of grandparent
        if (parent == grandparent->left) {
            fl_node* uncle = grandparent->right;

            // Case 1: Uncle is red, recoloring needed
            if (uncle != NULL && uncle->color == RED) {
                grandparent->color = RED;
                parent->color = BLACK;
                uncle->color = BLACK;
                node = grandparent;
            } else {
                // Case 2: Node is right child of its parent, left rotate needed
                if (node == parent->right) {
                    fl_left_rotate(parent, tree);
                    node = parent;
                    parent = node->parent;
                }

                // Case 3: Node is left child of its parent, right rotate needed
                fl_right_rotate(grandparent, tree);
                parent->color = BLACK;
                grandparent->color = RED;
                node = parent;
            }
        }
        // Parent is the right child of grandparent
        else {
            fl_node* uncle = grandparent->left;

            // Case 1: Uncle is red, recoloring needed
            if (uncle != NULL && uncle->color == RED) {
                grandparent->color = RED;
                parent->color = BLACK;
                uncle->color = BLACK;
                node = grandparent;
            } else {
                // Case 2: Node is left child of its parent, right rotate needed
                if (node == parent->left) {
                    fl_right_rotate(parent, tree);
                    node = parent;
                    parent = node->parent;
                }

                // Case 3: Node is right child of its parent, left rotate needed
                fl_left_rotate(grandparent, tree);
                parent->color = BLACK;
                grandparent->color = RED;
                node = parent;
            }
        }
    }

    // Ensure the root is always black
    tree->root->color = BLACK;
}

bool fl_tree_insert(fl_tree* tree, char* symbol, char* translated, char* code) {
    assert(tree != NULL);
    assert(symbol != NULL);

    fl_node* current = tree->root;
    fl_node* parent = NULL;

    int cmp_res;

    while (current != NULL) {
        parent = current;
        cmp_res = strcmp(current->symbol, symbol);
        if (cmp_res < 0) {
            current = current->right;
        } else if (cmp_res > 0) {
            current = current->left;
        } else {
            fprintf(stderr, "Symbol already in table!\n");
            return false;
        }
    }

    current = fl_tree_alloc_node(parent, symbol, translated, code);
    if (current == NULL) {
        fprintf(stderr, "Mer err in table!\n");
        return false;
    }

    if (parent == NULL) {
        tree->root = current;
    }

    else if (cmp_res < 0) {
        parent->right = current;
    } else {
        parent->left = current;
    }

    fl_tree_fix_insert(current, tree);

    return true;
}

fl_data* fl_tree_find(fl_tree* tree, const char* symbol) {
    assert(tree != NULL);
    assert(symbol != NULL);

    fl_node** node = &(tree->root);
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

void fl_tree_free(fl_tree* tree) {
    assert(tree != NULL);

    fl_node* curr = tree->root;
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
        fl_node* parent = curr->parent;
        free(curr);
        curr = parent;
    }
    tree->root = NULL;
}

char* fl_tree_use(fl_tree* tree, const char* func_id) {
    fl_data* data = fl_tree_find(tree, func_id);
    assert(data != NULL);
    data->counter++;
    return data->translated;
}

void fl_tree_include_rec(fl_node* node, FILE* fptr){
    if(node == NULL) return;
    if(node->data.counter > 0){
        fprintf(fptr, 
            "%s\n"
        , node->data.code);
    }
    fl_tree_include_rec(node->right, fptr);
    fl_tree_include_rec(node->left, fptr);
}

void fl_tree_include(fl_tree* tree, FILE* fptr) {
    assert(tree != NULL);

    fl_node* curr = tree->root;
    fl_tree_include_rec(curr, fptr);
}

error_code fl_tree_insert_functions(fl_tree* tree){
    if ( fl_tree_insert(tree, "ifj.write", "ifj$write",
        "LABEL ifj$write\n"
        "POPS GF@null\n"
        "WRITE GF@null\n"
        "RETURN\n"
    ) &&

    fl_tree_insert(tree, "ifj.readstr", "ifj$readstr",
        "LABEL ifj$readstr\n"
        "READ GF@null string\n"
        "PUSHS GF@null\n"
        "RETURN\n"
    ) &&

    fl_tree_insert(tree, "ifj.readi32", "ifj$readi32",
        "LABEL ifj$readi32\n"
        "READ GF@null int\n"
        "PUSHS GF@null\n"
        "RETURN\n"
    ) &&

    fl_tree_insert(tree, "ifj.readf64", "ifj$readf64",
        "LABEL ifj$readf64\n"
        "READ GF@null float\n"
        "PUSHS GF@null\n"
        "RETURN\n"
    ) &&

    fl_tree_insert(tree, "ifj.i2f", "ifj$i2f",
        "LABEL ifj$i2f\n"
        "INT2FLOATS\n"
        "RETURN\n"
    ) &&

    fl_tree_insert(tree, "ifj.f2i", "ifj$f2i",
        "LABEL ifj$f2i\n"
        "FLOAT2INTS\n"
        "RETURN\n"
    ) &&

    fl_tree_insert(tree, "ifj.string", "ifj$string",
        "LABEL ifj$string\n"
        "RETURN\n"
    ) &&

    fl_tree_insert(tree, "ifj.length", "ifj$length",
        "LABEL ifj$length\n"
        "POPS GF@null\n"
        "STRLEN GF@null GF@null\n"
        "PUSHS GF@null\n"
        "RETURN\n"
    ) &&

    fl_tree_insert(tree, "ifj.concat", "ifj$concat",
        "LABEL ifj$concat\n"
        "CREATEFRAME\n"
        "DEFVAR TF@s2\n"
        "POPS TF@s2\n"
        "DEFVAR TF@s1\n"
        "POPS TF@s1\n"
        "PUSHFRAME\n"
        "CONCAT LF@s1 LF@s1 LF@s2\n"
        "PUSHS LF@s1\n"
        "POPFRAME\n"
        "RETURN\n"
    ) &&

    fl_tree_insert(tree, "ifj.substring", "ifj$substring",
        "LABEL ifj$substring\n"
        "CREATEFRAME\n"
        "DEFVAR TF@j\n"
        "POPS TF@j\n"
        "DEFVAR TF@i\n"
        "POPS TF@i\n"
        "DEFVAR TF@s\n"
        "POPS TF@s\n"
        "DEFVAR TF@lenght\n"
        "DEFVAR TF@final\n"
        "DEFVAR TF@char\n"
        "PUSHFRAME\n"
        "PUSHS LF@i\n"
        "PUSHS int@0\n"
        "LTS\n"
        "PUSHS bool@true\n"
        "JUMPIFEQS ifj$substring$err\n"
        "PUSHS LF@j\n"
        "PUSHS int@0\n"
        "LTS\n"
        "PUSHS bool@true\n"
        "JUMPIFEQS ifj$substring$err\n"
        "PUSHS LF@i\n"
        "PUSHS LF@j\n"
        "GTS\n"
        "PUSHS bool@true\n"
        "JUMPIFEQS ifj$substring$err\n"
        "STRLEN LF@lenght LF@s\n"
        "PUSHS LF@i\n"
        "PUSHS LF@lenght\n"
        "LTS\n"
        "PUSHS bool@false\n"
        "JUMPIFEQS ifj$substring$err\n"
        "PUSHS LF@j\n"
        "PUSHS LF@lenght\n"
        "GTS\n"
        "PUSHS bool@true\n"
        "JUMPIFEQS ifj$substring$err\n"
        "MOVE LF@final string@\n"
        "LABEL ifj$substring$loop\n"
        "JUMPIFEQ ifj$substring$end LF@i LF@j\n"
        "GETCHAR LF@char LF@s LF@i\n"
        "CONCAT LF@final LF@final LF@char\n"
        "ADD LF@i LF@i int@1\n"
        "JUMP ifj$substring$loop\n"
        "LABEL ifj$substring$end\n"
        "PUSHS LF@final\n"
        "POPFRAME\n"
        "RETURN\n"
        "LABEL ifj$substring$err\n"
        "PUSHS nil@nil\n"
        "POPFRAME\n"
        "RETURN\n"    
    ) &&

    fl_tree_insert(tree, "ifj.strcmp", "ifj$strcmp", 
        "LABEL ifj$strcmp\n"
        "CREATEFRAME\n"
        "DEFVAR TF@s1\n"
        "POPS TF@s1\n"
        "DEFVAR TF@s2\n"
        "POPS TF@s2\n"
        "DEFVAR TF@result\n"
        "DEFVAR TF@len1\n"
        "DEFVAR TF@len2\n"
        "DEFVAR TF@i\n"
        "DEFVAR TF@char1\n"
        "DEFVAR TF@char2 \n"
        "PUSHFRAME\n"
        "STRLEN LF@len1 LF@s1\n"
        "STRLEN LF@len2 LF@s2\n"
        "MOVE LF@i int@0\n"
        "LABEL strcmp$loop\n"
        "JUMPIFEQ strcmp$done LF@i LF@len1\n"
        "JUMPIFEQ strcmp$done LF@i LF@len2\n"
        "GETCHAR LF@char1 LF@s1 LF@i\n"
        "GETCHAR LF@char2 LF@s2 LF@i\n"
        "JUMPIFNEQ strcmp$diff LF@char1 LF@char2\n"
        "ADD LF@i LF@i int@1\n"
        "JUMP strcmp$loop\n"
        "LABEL strcmp$diff\n"
        "LT LF@result LF@char1 LF@char2\n"
        "JUMPIFEQ strcmp$return_minus1 LF@result bool@true\n"
        "GT LF@result LF@char1 LF@char2\n"
        "JUMPIFEQ strcmp$return_plus1 LF@result bool@true\n"
        "LABEL strcmp$done\n"
        "LT LF@result LF@len1 LF@len2\n"
        "JUMPIFEQ strcmp$return_minus1 LF@result bool@true\n"
        "GT LF@result LF@len1 LF@len2\n"
        "JUMPIFEQ strcmp$return_plus1 LF@result bool@true\n"
        "MOVE LF@result int@0\n"
        "JUMP strcmp$return\n"
        "LABEL strcmp$return_minus1\n"
        "MOVE LF@result int@1\n"
        "JUMP strcmp$return\n"
        "LABEL strcmp$return_plus1\n"
        "MOVE LF@result int@-1\n"
        "LABEL strcmp$return\n"
        "PUSHS LF@result \n"
        "POPFRAME\n"
        "RETURN\n"
    ) &&

    fl_tree_insert(tree, "ifj.ord", "ifj$ord",
        "LABEL ifj$ord\n"
        "CREATEFRAME\n"
        "DEFVAR TF@i\n"
        "POPS TF@i\n"
        "DEFVAR TF@s\n"
        "POPS TF@s\n"
        "DEFVAR TF@result\n"
        "DEFVAR TF@ret\n"
        "DEFVAR TF@len\n"
        "PUSHFRAME\n"
        "STRLEN LF@len LF@s\n"
        "MOVE LF@result int@0\n"
        "LT LF@ret LF@i int@0\n"
        "JUMPIFEQ ord$return LF@ret bool@true\n"
        "LT LF@ret LF@i LF@len\n"
        "NOT LF@ret LF@ret\n"
        "JUMPIFEQ ord$return LF@ret bool@true\n"
        "STRI2INT LF@result LF@s LF@i\n"
        "LABEL ord$return\n"
        "PUSHS LF@result\n"
        "POPFRAME\n"
        "RETURN\n"
    ) &&

    fl_tree_insert(tree, "ifj.chr", "ifj$chr",
        "LABEL ifj$chr\n"
        "INT2CHARS\n"
        "RETURN\n"
    ) &&

    fl_tree_insert(tree, "bld.orelse", "$bld$orelse", 
        "LABEL $bld$orelse\n"
        "CREATEFRAME\n"
        "DEFVAR TF@elsevalue\n"
        "POPS TF@elsevalue\n"
        "DEFVAR TF@ifvalue\n"
        "POPS TF@ifvalue\n"
        "PUSHFRAME\n"
        "JUMPIFEQ $bld$orelse$else LF@ifvalue nil@nil\n"
        "PUSHS LF@ifvalue\n"
        "POPFRAME\n"
        "RETURN\n"
        "LABEL $bld$orelse$else\n"
        "PUSHS LF@elsevalue\n"
        "POPFRAME\n"
        "RETURN\n"
    ) &&

    fl_tree_insert(tree, "bld.orelse_unreachable", "$bld$orelse$unreachable", 
        "LABEL $bld$orelse$unreachable\n"
        "CREATEFRAME\n"
        "DEFVAR TF@ifvalue\n"
        "POPS TF@ifvalue\n"
        "PUSHFRAME\n"
        "JUMPIFEQ $bld$orelse$unreachable$else LF@ifvalue nil@nil\n"
        "PUSHS LF@ifvalue\n"
        "POPFRAME\n"
        "RETURN\n"
        "LABEL $bld$orelse$unreachable$else\n"
        "WRITE string@panic:\\032reached\\032unreachable\\032code\\010\n"
        "EXIT int@57\n"
    )){
        return NO_ERR;
    }
    else {
        return INTERNAL_ERR;
    }
}