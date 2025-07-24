// Projekt: Implementace překladače imperativního jazyka IFJ24
// Autor: Marek Slaný   (xslany03)

#include "semantic.h"
#include "ast.h"
#include "data_types.h"
#include "error.h"
#include "scope_stack.h"
#include "symtable.h"
#include "util.h"
#include "vector.h"
#include <assert.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Recursively checks whether all the variables have been used and muted
error_code check_variable_usage(struct rb_tree_node* node) {
    error_code err;
    if (node == NULL) {
        return NO_ERR;
    }
    DEBUG_PRINT("Checking usage of %s", node->symbol);

    if (node->left != NULL) {
        err = check_variable_usage(node->left);
        // One error is enough
        if (err != NO_ERR) {
            return err;
        }
    }
    if (node->right != NULL) {
        err = check_variable_usage(node->right);
        // One error is enough
        if (err != NO_ERR) {
            return err;
        }
    }

    if (node->data.used) {
        DEBUG_PRINT("Symbol %s has been used", node->symbol);
    }
    if (node->data.muted) {
        DEBUG_PRINT("Symbol %s has been muted", node->symbol);
    }

    if (node->data.symbol_type == SYM_CONST && !node->data.used) {
        return SEM_UNUSED_ERR;
    }
    if (node->data.symbol_type == SYM_VAR && !node->data.muted) {
        return SEM_UNUSED_ERR;
    }

    return NO_ERR;
}

// Enter a new scope by pushing a new symbol table onto the 'symtable_stack'
// symbols declare in the current scope will always be pushed to this
error_code enter_scope(vector* symtable_stack) {
    b_tree* symtable = malloc(sizeof(b_tree));

    // Check whether the memory was allocated succesfully
    if (symtable == NULL)
        return INTERNAL_ERR;

    rb_tree_init(symtable);

    vec_push(symtable_stack, symtable);

    return NO_ERR;
}

// Leaves a scope by popping topmost symtable from the 'symtable_stack' thus
// invalidating all the symbols declared in the current scope.
error_code leave_scope(vector* symtable_stack) {
    b_tree* symtable = (b_tree*)vec_pop(symtable_stack);
    error_code err = check_variable_usage(symtable->root);
    rb_tree_free(symtable);
    free(symtable);
    return err;
}

// Finds a symbol in the symtable and returns a pointer to the asociated data
// or NULL if the symbol is not declared anywhere.
// The caller should take care when subsequently calling 'leave_scope()',
// as this might invalidate the pointer, thus 'find_symbol()' should be called
// again for the same symbol, should you need to acces it after calling
// 'leave_scope()'.
b_tree_data* find_symbol(vector* symtable_stack, const char* symbol) {
    DEBUG_PRINT("Finding symbol: %s", symbol);
    assert(symbol != NULL);
    for (int i = symtable_stack->len - 1; i >= 0; i--) {
        b_tree* symtable = vec_get(symtable_stack, i);
        b_tree_data* data = rb_tree_find(symtable, symbol);
        if (data != NULL) {
            return data;
        }
    }
    return NULL;
}

// Declares a symbol in the current scope (topmost symtable).
// returns the result 'rb_tree_insert()'.
int declare_symbol(vector* symtable_stack, char* symbol, b_tree_data data) {

    if (symbol == NULL) {
        ERROR_PRINT("Trying to declare a symbol that is NULL%s", "");
        return INTERNAL_ERR;
    }
    b_tree* symtable;

    char* dup_sym = d_string(symbol);

    switch (data.symbol_type) {
    case SYM_VAR:
        DEBUG_PRINT("Declaring variable: %s (%s)", dup_sym,
                    DATA_TYPE_STRINGS[data.return_type]);
        break;
    case SYM_CONST:
        DEBUG_PRINT("Declaring const: %s (%s)", dup_sym,
                    DATA_TYPE_STRINGS[data.return_type]);
        break;
    case SYM_FUNC:
        DEBUG_PRINT("Declaring function: %s -> %s", dup_sym,
                    DATA_TYPE_STRINGS[data.return_type]);
        break;
    }

    // Check if the symbol is declared in any of the previous scopes
    for (size_t i = 0; i < symtable_stack->len - 1; i++) {
        symtable = vec_get(symtable_stack, i);

        if (rb_tree_find(symtable, symbol) != NULL) {
            ERROR_PRINT("Redefinition of %s", symbol);
            return INSERT_ALREADY_IN;
        }
    }

    symtable = vec_get(symtable_stack, symtable_stack->len - 1);

    if (dup_sym == NULL) {
        return INSERT_MEM_ERR;
    }

    int res = rb_tree_insert(symtable, dup_sym, data);
    if (res == INSERT_ALREADY_IN) {
        ERROR_PRINT("Redefinition of %s", dup_sym);
    }

    return res;
}

// Determines if the passed 'n' is a constant expression, can only be used
// on LITERAL and VAR nodes
bool expr_is_constant(node* n, vector* symtable_stack) {
    assert(n->data.type == LITERAL || n->data.type == VAR);
    if (n->data.type == LITERAL) {
        return true;
    }
    b_tree_data* data = find_symbol(symtable_stack, n->data.id_name);
    if (data->symbol_type == SYM_CONST) {
        DEBUG_PRINT("Symbol %s is constant", n->data.id_name);
        return true;
    }
    DEBUG_PRINT("Symbol %s is not constant", n->data.id_name);
    return false;
}

// Adds the builtin functions to the symtable, should be called before
// running semantic analysis, returns result of 'declare_symbol()'.
int populate_with_builtins(vector* symtable_stack) {
    data_type d;
    b_tree_data data;
    int res;
    data.symbol_type = SYM_FUNC;

    // The read functions don't have any parameters
    data.func_parameters = NULL;

    // ifj.readstr() ?[]u8
    data.return_type = U8_SLICE_OPTIONAL;
    res = declare_symbol(symtable_stack, "ifj.readstr", data);
    if (res != 0) {
        return res;
    }

    // ifj.readi32() ?i32
    data.return_type = I32_OPTIONAL;
    res = declare_symbol(symtable_stack, "ifj.readi32", data);
    if (res != 0) {
        return res;
    }

    // ifj.readf64() ?f64
    data.return_type = F64_OPTIONAL;
    res = declare_symbol(symtable_stack, "ifj.readf64", data);
    if (res != 0) {
        return res;
    }

    // ifj.write(term) void
    data.return_type = VOID;
    data.func_parameters = vec_init(1, sizeof(data_type));
    if (data.func_parameters == NULL) {
        return INSERT_MEM_ERR;
    }
    d = UNKNOWN;
    vec_push(data.func_parameters, &d);
    res = declare_symbol(symtable_stack, "ifj.write", data);
    if (res != 0) {
        return res;
    }

    // ifj.i2f(term : i32) f64
    data.return_type = F64;
    data.func_parameters = vec_init(1, sizeof(data_type));
    if (data.func_parameters == NULL) {
        return INSERT_MEM_ERR;
    }
    d = I32;
    vec_push(data.func_parameters, &d);
    res = declare_symbol(symtable_stack, "ifj.i2f", data);
    if (res != 0) {
        return res;
    }

    // ifj.f2i(term : f64) i32
    data.return_type = I32;
    data.func_parameters = vec_init(1, sizeof(data_type));
    if (data.func_parameters == NULL) {
        return INSERT_MEM_ERR;
    }
    d = F64;
    vec_push(data.func_parameters, &d);
    res = declare_symbol(symtable_stack, "ifj.f2i", data);
    if (res != 0) {
        return res;
    }

    // ifj.string(term) []u8
    data.return_type = U8_SLICE;
    data.func_parameters = vec_init(1, sizeof(data_type));
    if (data.func_parameters == NULL) {
        return INSERT_MEM_ERR;
    }
    d = UNKNOWN;
    vec_push(data.func_parameters, &d);
    res = declare_symbol(symtable_stack, "ifj.string", data);
    if (res != 0) {
        return res;
    }

    // ifj.length(s: []u8) i32
    data.return_type = I32;
    data.func_parameters = vec_init(1, sizeof(data_type));
    if (data.func_parameters == NULL) {
        return INSERT_MEM_ERR;
    }
    d = U8_SLICE;
    vec_push(data.func_parameters, &d);
    res = declare_symbol(symtable_stack, "ifj.length", data);
    if (res != 0) {
        return res;
    }

    // ifj.concat(s1: []u8, s2: []u8) []u8
    data.return_type = U8_SLICE;
    data.func_parameters = vec_init(2, sizeof(data_type));
    if (data.func_parameters == NULL) {
        return INSERT_MEM_ERR;
    }
    d = U8_SLICE;
    vec_push(data.func_parameters, &d);
    d = U8_SLICE;
    vec_push(data.func_parameters, &d);
    res = declare_symbol(symtable_stack, "ifj.concat", data);
    if (res != 0) {
        return res;
    }

    // ifj.substring(s: []u8, i: i32, j: i32) ?[]u8
    data.return_type = U8_SLICE_OPTIONAL;
    data.func_parameters = vec_init(3, sizeof(data_type));
    if (data.func_parameters == NULL) {
        return INSERT_MEM_ERR;
    }
    d = U8_SLICE;
    vec_push(data.func_parameters, &d);
    d = I32;
    vec_push(data.func_parameters, &d);
    d = I32;
    vec_push(data.func_parameters, &d);
    res = declare_symbol(symtable_stack, "ifj.substring", data);
    if (res != 0) {
        return res;
    }

    // ifj.strcmp(s1: []u8, s2: []u8) i32
    data.return_type = I32;
    data.func_parameters = vec_init(2, sizeof(data_type));
    if (data.func_parameters == NULL) {
        return INSERT_MEM_ERR;
    }
    d = U8_SLICE;
    vec_push(data.func_parameters, &d);
    d = U8_SLICE;
    vec_push(data.func_parameters, &d);
    res = declare_symbol(symtable_stack, "ifj.strcmp", data);
    if (res != 0) {
        return res;
    }

    // ifj.ord(s: []u8, i: i32) i32
    data.return_type = I32;
    data.func_parameters = vec_init(2, sizeof(data_type));
    if (data.func_parameters == NULL) {
        return INSERT_MEM_ERR;
    }
    d = U8_SLICE;
    vec_push(data.func_parameters, &d);
    d = I32;
    vec_push(data.func_parameters, &d);
    res = declare_symbol(symtable_stack, "ifj.ord", data);
    if (res != 0) {
        return res;
    }

    // ifj.chr(i: i32) []u8
    data.return_type = U8_SLICE;
    data.func_parameters = vec_init(1, sizeof(data_type));
    if (data.func_parameters == NULL) {
        return INSERT_MEM_ERR;
    }
    d = I32;
    vec_push(data.func_parameters, &d);
    res = declare_symbol(symtable_stack, "ifj.chr", data);
    if (res != 0) {
        return res;
    }

    return INSERT_SUCCESS;
}

// Takes node 'n' and replaces it with call to function 'i2f()' with 'n' as the
// parameter.
error_code call_i2f(node* n) {
    node* func = create_node(FUNC);
    if (func == NULL) {
        ERROR_PRINT("Could not create node for the function%s", "");
        return INTERNAL_ERR;
    }

    DEBUG_PRINT("Converting I32 to F64%s", "");
    func->data.id_name = d_string("ifj.i2f");
    func->data.ret_value = F64;

    // Replace 'n' in it's parent with the function
    node* p = n->ancestor;
    for (int i = 0; i < p->child_count; i++) {
        if (p->children[i] == n) {
            p->children[i] = func;
            break;
        }
    }
    n->ancestor = NULL;

    node* param_list = create_node(INPUT_PARAM_LIST);
    node* param = create_node(INPUT_PARAM);
    node* expr = create_node(EXPR);
    expr->data.ret_value = n->data.ret_value;
    param->data.ret_value = n->data.ret_value;

    if (param_list == NULL || param == NULL || expr == NULL) {
        ERROR_PRINT("Error while allocating nodes for the function%s", "");
        return INTERNAL_ERR;
    }
    bool err;
    DEBUG_PRINT("Appending function for conversion%s", "");
    err = !append_node(param_list, func);
    err = err || !append_node(param, param_list);
    err = err || !append_node(expr, param);
    err = err || !append_node(n, expr);

    if (err) {
        ERROR_PRINT("Error while appending nodes for the function%s", "");
        return INTERNAL_ERR;
    }
    return NO_ERR;
}

// Takes node 'n' and replaces it with call to function 'f2i()' with 'n' as the
// parameter.
error_code call_f2i(node* n) {
    node* func = create_node(FUNC);
    if (func == NULL) {
        ERROR_PRINT("Could not create node for the function%s", "");
        return INTERNAL_ERR;
    }

    DEBUG_PRINT("Converting F64 to I32%s", "");
    // f2i
    func->data.id_name = d_string("ifj.f2i");
    func->data.ret_value = I32;

    // Replace 'n' in it's parent with the function
    node* p = n->ancestor;
    for (int i = 0; i < p->child_count; i++) {
        if (p->children[i] == n) {
            p->children[i] = func;
            break;
        }
    }
    n->ancestor = NULL;

    node* param_list = create_node(INPUT_PARAM_LIST);
    node* param = create_node(INPUT_PARAM);
    node* expr = create_node(EXPR);

    if (param_list == NULL || param == NULL || expr == NULL) {
        ERROR_PRINT("Error while allocating nodes for the function%s", "");
        return INTERNAL_ERR;
    }
    expr->data.ret_value = n->data.ret_value;
    param->data.ret_value = n->data.ret_value;

    bool err;
    err = !append_node(param_list, func);
    err = err || !append_node(param, param_list);
    err = err || !append_node(expr, param);
    err = err || !append_node(n, expr);

    if (err) {
        ERROR_PRINT("Error while appending nodes for the function%s", "");
        return INTERNAL_ERR;
    }
    return NO_ERR;
}

// Tries to convert the node variable to the target type
// returns 'SEM_COMPATIBILITY_ERR' if it fails
error_code implicit_conversion(node* n, data_type d, vector* symtable_stack) {
    bool n_const = false;
    data_type n_t = n->data.ret_value;
    // No need to convert
    if (n_t == d) {
        return NO_ERR;
    }
    // Cannot convert anything other than I32 or F64
    if (!(n_t == I32 || n_t == F64)) {
        ERROR_PRINT("Cant convert %s -> %s",
                    DATA_TYPE_STRINGS[n->data.ret_value], DATA_TYPE_STRINGS[d]);
        return SEM_COMPATIBILITY_ERR;
    }

    if (n->data.type == LITERAL) {
        n_const = true;
    } else if (n->data.type == VAR) {
        b_tree_data* data = find_symbol(symtable_stack, n->data.id_name);
        if (data->symbol_type == SYM_CONST) {
            n_const = true;
        }
    }

    // Convert a
    if (n_const) {

        if (n_t == I32) {
            return call_i2f(n);
        }
        return call_f2i(n);
    }

    return SEM_COMPATIBILITY_ERR;
}

// Handles type conversion, 'ast' is a node of binary operator.
// If the types of the children of 'ast' are incompatible tries
// converting them.
error_code handle_type_conversion(node* ast, vector* symtable_stack) {
    node* n1 = ast->children[0];
    node* n2 = ast->children[1];

    // Types are compatible and no need to convert
    if (n1->data.ret_value == n2->data.ret_value) {
        return NO_ERR;
    }
    DEBUG_PRINT("Converting type...%s", "");

    // We need to convert
    node* to_convert;
    switch (ast->data.type) {
        // Logical operators
    case EQUAL:
    case NOT_EQUAL:
    case EQUALMORE:
    case EQUALLESS:
    case MORE:
    case LESS:
        if (expr_is_constant(n1, symtable_stack)) {
            to_convert = n1;
        } else if (expr_is_constant(n2, symtable_stack)) {
            to_convert = n2;
        } else {
            // Cannot convert non-constant expressions
            return SEM_COMPATIBILITY_ERR;
        }

        // Possible conversion of i32 -> f64
        if (to_convert->data.ret_value == I32 &&
            to_convert->data.type == LITERAL) {
            DEBUG_PRINT("Converting i32 -> f64 %s",
                        NODE_TYPE_STRINGS[to_convert->data.type]);
            return call_i2f(to_convert);
        }

        // Possible conversion of f64 -> i32 if the value
        // does not have any decimal places
        if (to_convert->data.ret_value == F64 &&
            to_convert->data.type == LITERAL &&
            !has_decimals(to_convert->data.float_value)) {
            DEBUG_PRINT("Converting f64 -> i32 %s",
                        NODE_TYPE_STRINGS[to_convert->data.type]);
            return call_f2i(to_convert);
        }

        break;
        // Arithmetic operators
    case ADD:
    case SUB:
    case MUL:
        if (expr_is_constant(n1, symtable_stack) &&
            expr_is_constant(n2, symtable_stack)) {
            // Both operands can be converted, we need to pick one
            // TODO: determine which literal to convert
            to_convert = n1;
        } else if (expr_is_constant(n1, symtable_stack)) {
            to_convert = n1;
        } else if (expr_is_constant(n2, symtable_stack)) {
            to_convert = n2;
        } else {
            // Cannot convert non-constant expressions
            return SEM_COMPATIBILITY_ERR;
        }

        // Possible conversion of i32 -> f64
        if (to_convert->data.ret_value == I32 &&
            to_convert->data.type == LITERAL) {
            DEBUG_PRINT("Converting i32 -> f64 %s",
                        NODE_TYPE_STRINGS[to_convert->data.type]);
            return call_i2f(to_convert);
        }
        // Possible conversion of f64 -> i32 if the value
        // does not have any decimal places
        if (to_convert->data.ret_value == F64 &&
            to_convert->data.type == LITERAL &&
            !has_decimals(to_convert->data.float_value)) {
            DEBUG_PRINT("Converting f64 -> i32 %s",
                        NODE_TYPE_STRINGS[to_convert->data.type]);
            return call_f2i(to_convert);
        }

        // Cannot convert the type
        return SEM_COMPATIBILITY_ERR;
    case DIV:
        if (expr_is_constant(n1, symtable_stack) &&
            expr_is_constant(n2, symtable_stack)) {
            // Both operands can be converted, we need to pick one
            // TODO: determine which literal to convert
            to_convert = n1;
        } else if (expr_is_constant(n1, symtable_stack)) {
            to_convert = n1;
        } else if (expr_is_constant(n2, symtable_stack)) {
            to_convert = n2;
        } else {
            // Cannot convert non-constant expressions
            return SEM_COMPATIBILITY_ERR;
        }

        // Possible conversion of f64 -> i32 if the value
        // does not have any decimal places
        if (to_convert->data.ret_value == F64 &&
            to_convert->data.type == LITERAL &&
            !has_decimals(to_convert->data.float_value)) {
            DEBUG_PRINT("Converting f64 -> i32 %s",
                        NODE_TYPE_STRINGS[to_convert->data.type]);
            return call_f2i(to_convert);
        }

        // Cannot convert the type
        return SEM_COMPATIBILITY_ERR;
    default:
        break;
    }

    // We shouldn't ever get here
    ERROR_PRINT(
        "handle_type_compatibility() - invalid node passed as argument%s", "");
    return INTERNAL_ERR;
}

error_code var(node* ast, vector* symtable_stack, scope_stack* scp_s) {
    b_tree_data* data = find_symbol(symtable_stack, ast->data.id_name);

    if (data == NULL) {
        return SEM_UNDEFINED_ERR;
    }
    if (data->symbol_type == SYM_FUNC) {
        return SEM_VARIABLE_ERR;
    }
    if (!data->initialized) {
        return SEM_VARIABLE_ERR;
    }

    data->used = true;
    ast->data.ret_value = data->return_type;
    ast->data.scope_id = d_string(data->scope_id);

    return NO_ERR;
}

// Checks variable declaration.
error_code declaration(node* ast, vector* symtable_stack, scope_stack* scp_s) {
    b_tree_data data;

    // Determine the type of the symbol
    switch (ast->data.type) {
    case VAR_DEF:
        data.symbol_type = SYM_VAR;
        break;
    case CONST_DEF:
        data.symbol_type = SYM_CONST;
        break;
    default:
        // This is unreachable with intended usage.
        assert(false);
    }

    data.return_type = ast->data.ret_value;
    data.used = false;
    data.muted = false;

    node* expr = get_child_by_type(ast, EXPR, 0);

    if (data.return_type == UNKNOWN) {
        // No way to determine the type, or the type cannot be used in
        // assignment
        if (expr == NULL || !type_is_assignable(expr->data.ret_value) ||
            expr->data.ret_value == DT_NULL) {
            return SEM_INFERENCE_ERR;
        }
        // Determine the type from the expression.
        data.return_type = expr->data.ret_value;
        data.initialized = true;
    } else {
        if (expr == NULL) {
            data.initialized = false;
            // Const must always be initialized;
            if (data.symbol_type == SYM_CONST) {
                return SEM_VARIABLE_ERR;
            }
        } else {
            if (!type_is_assignable(expr->data.ret_value)) {
                return SEM_INFERENCE_ERR;
            }
            data.initialized = true;
            // Check whether the expression type and the specified type are
            // the same.
            if (type_is_optional(data.return_type)) {
                if (unwrap_optional(data.return_type) != expr->data.ret_value &&
                    expr->data.ret_value != DT_NULL &&
                    expr->data.ret_value != data.return_type) {
                    ERROR_PRINT("Invalid type %s, cannot be asigned to a "
                                "symbol of type %s",
                                DATA_TYPE_STRINGS[expr->data.ret_value],
                                DATA_TYPE_STRINGS[data.return_type]);
                    return SEM_COMPATIBILITY_ERR;
                }
            } else if (expr->data.ret_value != data.return_type) {
                ERROR_PRINT("Invalid type %s, cannot be asigned to a "
                            "symbol of type %s",
                            DATA_TYPE_STRINGS[expr->data.ret_value],
                            DATA_TYPE_STRINGS[data.return_type]);
                return SEM_COMPATIBILITY_ERR;
            }
        }
    }

    ast->data.scope_id = get_scope_id(scp_s);
    data.scope_id = d_string(ast->data.scope_id);
    int res = declare_symbol(symtable_stack, ast->data.id_name, data);
    if (res == INSERT_ALREADY_IN) {
        return SEM_VARIABLE_ERR;
    }
    if (res == INSERT_MEM_ERR) {
        return INTERNAL_ERR;
    }

    return NO_ERR;
}

error_code bitwise_operators(node* ast, vector* symtable_stack) {
    // The node is binary operation so it has just two children
    data_type type0 = ast->children[0]->data.ret_value;
    data_type type1 = ast->children[1]->data.ret_value;

    // Bitwise operations are not possible on optionals.
    if (type_is_optional(type0) || type_is_optional(type1)) {
        return SEM_COMPATIBILITY_ERR;
    }

    // Bitwise operations are not possible on strings
    if (type0 == U8_SLICE || type1 == U8_SLICE) {
        return SEM_COMPATIBILITY_ERR;
    }

    if (type0 != type1) {
        DEBUG_PRINT("Trying implicit conversion%s", "");
        error_code err = handle_type_conversion(ast, symtable_stack);
        if (err != NO_ERR) {
            return err;
        }
        // Update the types
        type0 = ast->children[0]->data.ret_value;
        type1 = ast->children[1]->data.ret_value;
    }

    ast->data.ret_value = type0;

    return NO_ERR;
}

// Check arithmetic operations.
error_code arith_operators(node* ast, vector* symtable_stack) {
    // The node is binary operation so it has just two children
    data_type type0 = ast->children[0]->data.ret_value;
    data_type type1 = ast->children[1]->data.ret_value;

    // No strings allowed in arithmetic operations.
    if (type0 == U8_SLICE || type1 == U8_SLICE) {
        return SEM_COMPATIBILITY_ERR;
    }

    // Arithmetic operations are not possible on optionals.
    if (type_is_optional(type0) || type_is_optional(type1)) {
        return SEM_COMPATIBILITY_ERR;
    }

    if (type0 != type1) {
        DEBUG_PRINT("Trying implicit conversion%s", "");
        error_code err = handle_type_conversion(ast, symtable_stack);
        if (err != NO_ERR) {
            return err;
        }
        // Update the types
        type0 = ast->children[0]->data.ret_value;
        type1 = ast->children[1]->data.ret_value;
    }

    ast->data.ret_value = type0;
    return NO_ERR;
}

// Check logical operations.
error_code logic_operators(node* ast, vector* symtable_stack) {
    // The node is binary operation so it has just two children
    data_type type0 = ast->children[0]->data.ret_value;
    data_type type1 = ast->children[1]->data.ret_value;
    ast->data.ret_value = BOOLEAN;

    if (type0 != type1) {
        DEBUG_PRINT("Trying implicit conversion%s", "");
        error_code err = handle_type_conversion(ast, symtable_stack);
        if (err != NO_ERR) {
            return err;
        }
        // Update the types
        type0 = ast->children[0]->data.ret_value;
        type1 = ast->children[1]->data.ret_value;
    }
    return NO_ERR;
}

error_code assignment(node* ast, vector* symtable_stack, scope_stack* scp_s) {
    // Throwaway asignment, has no way to produce errors.
    if (ast->data.type == ASSIGN_THROW_AWAY)
        return NO_ERR;

    b_tree_data* data = find_symbol(symtable_stack, ast->data.id_name);

    // Variable is not defined.
    if (data == NULL) {
        return SEM_UNDEFINED_ERR;
    }

    // Trying to assign to a const
    if (data->symbol_type == SYM_CONST) {
        return SEM_VARIABLE_ERR;
    }

    // Trying to assign to a function
    if (data->symbol_type == SYM_FUNC) {
        return SEM_UNDEFINED_ERR;
    }

    DEBUG_PRINT("Symbol %s has been muted", ast->data.id_name);
    data->muted = true;

    node* expr = get_child_by_type(ast, EXPR, 0);
    DEBUG_PRINT("Assigning to %s (%s) with expr of type %s", ast->data.id_name,
                DATA_TYPE_STRINGS[data->return_type],
                DATA_TYPE_STRINGS[expr->data.ret_value]);

    if (!type_is_assignable(expr->data.ret_value)) {
        return SEM_COMPATIBILITY_ERR;
    }

    // Asigning expression of incompatible type to non-optional variable.
    if (!type_is_optional(data->return_type) &&
        data->return_type != expr->data.ret_value) {
        error_code err =
            implicit_conversion(expr, data->return_type, symtable_stack);
        if (err != NO_ERR) {
            return err;
        }

    } else if (type_is_optional(data->return_type) &&
               !(unwrap_optional(data->return_type) == expr->data.ret_value ||
                 expr->data.ret_value == DT_NULL ||
                 data->return_type == expr->data.ret_value)) {
        // Asigning expression of incompatible type to optional variable
        DEBUG_PRINT("Optional with incompatible type%s", "");
        error_code err = implicit_conversion(
            expr, unwrap_optional(data->return_type), symtable_stack);
        if (err != NO_ERR) {
            return err;
        }
    }

    ast->data.scope_id = d_string(data->scope_id);

    // The variable is now initialized
    data->initialized = true;

    return NO_ERR;
}

// Sets the return value of the whole expression.
error_code expression(node* ast) {
    ast->data.ret_value = ast->children[0]->data.ret_value;
    return NO_ERR;
}

error_code param(node* ast, vector* symtable_stack, scope_stack* scp_s) {
    b_tree_data data;
    data.symbol_type = SYM_CONST;
    data.return_type = ast->data.ret_value;
    data.initialized = true;
    data.scope_id = get_scope_id(scp_s);
    ast->data.scope_id = d_string(data.scope_id);

    int res = declare_symbol(symtable_stack, ast->data.id_name, data);
    if (res == INSERT_ALREADY_IN) {
        return SEM_VARIABLE_ERR;
    }
    if (res == INSERT_MEM_ERR) {
        return INTERNAL_ERR;
    }

    return NO_ERR;
}

error_code while_def(node* ast, scope_stack* scp_s) {
    ast->data.scope_id = get_scope_id(scp_s);

    return NO_ERR;
}

error_code check_label(node* ast) {
    char* label = ast->data.label;
    node* ancestor = ast->ancestor;
    if (label == NULL)
        return NO_ERR;

    while (ancestor != NULL) {
        if (ancestor->data.type == WHILE) {
            if ((ancestor->data.label != NULL) &&
                (strcmp(ancestor->data.label, label) == 0)) {
                ast->data.scope_id = d_string(ancestor->data.scope_id);
            }
        }

        ancestor = ancestor->ancestor;
    }
    return SEM_UNDEFINED_ERR;
}

error_code function_call(node* ast, vector* symtable_stack) {
    DEBUG_PRINT("Finding function %s", ast->data.id_name);
    b_tree_data* data = find_symbol(symtable_stack, ast->data.id_name);

    // The function isn't defined
    if (data == NULL) {
        return SEM_UNDEFINED_ERR;
    }

    node* param_list = get_child_by_type(ast, INPUT_PARAM_LIST, 0);
    assert(param_list != NULL);
    node* param;
    int i;
    for (i = 0; i < param_list->child_count; i++) {
        param = get_child_by_type(param_list, INPUT_PARAM, i);

        if (param == NULL) {
            break;
        }
        // Trying to pass parameters to a function that has none.
        if (data->func_parameters == NULL) {
            ERROR_PRINT("Passing arguments to function that has none%s", "");
            return SEM_FUNCTION_ERR;
        }
        // Trying to pass more arguments than the funciton has.
        if (data->func_parameters->len <= i) {
            ERROR_PRINT("Got too many arguments%s", "");
            return SEM_FUNCTION_ERR;
        }
        data_type* d = vec_get(data->func_parameters, i);

        if (param->data.ret_value != *d) {
            // Argument can be of any type
            if (*d == UNKNOWN)
                continue;
            ERROR_PRINT("Expected param %s got %s instead",
                        DATA_TYPE_STRINGS[*d],
                        DATA_TYPE_STRINGS[param->data.ret_value]);
            return SEM_FUNCTION_ERR;
        }
    }
    // Missing arguments
    int expected_count;
    if (data->func_parameters == NULL) {
        expected_count = 0;
    } else {
        expected_count = data->func_parameters->len;
    }
    if (i + 1 < expected_count) {
        ERROR_PRINT("Not enough arguments provided: got %d, expected %lu",
                    i + 1, data->func_parameters->len);
        return SEM_FUNCTION_ERR;
    }

    ast->data.ret_value = data->return_type;

    if (ast->ancestor->data.type == STMT_LIST && data->return_type != VOID) {
        return SEM_FUNCTION_ERR;
    }

    return NO_ERR;
}

error_code cond(node* ast, vector* symtable_stack, scope_stack* scp_s) {
    node* null_cond = get_child_by_type(ast, NULL_COND, 0);
    node* expr = get_child_by_type(ast, EXPR, 0);

    // No null condition
    if (null_cond == NULL) {
        if (expr->data.ret_value != BOOLEAN)
            return SEM_COMPATIBILITY_ERR;
    } else {
        if (ast->ancestor->data.type == FOR) {
            if (expr->data.ret_value != U8_SLICE) {
                return SEM_COMPATIBILITY_ERR;
            }
            null_cond->data.ret_value = U8;
            null_cond->data.scope_id = get_scope_id(scp_s);

            b_tree_data data;
            data.initialized = true;
            data.return_type = U8;
            data.symbol_type = SYM_CONST;
            data.scope_id = d_string(null_cond->data.scope_id);
            int ret;
            ret = declare_symbol(symtable_stack, null_cond->data.id_name, data);
            if (ret == INSERT_ALREADY_IN)
                return SEM_VARIABLE_ERR;
            if (ret == INSERT_MEM_ERR)
                return INTERNAL_ERR;

            return NO_ERR;
        }
        if (!type_is_optional(expr->data.ret_value))
            return SEM_COMPATIBILITY_ERR;
        data_type r_type = unwrap_optional(expr->data.ret_value);
        null_cond->data.ret_value = r_type;
        null_cond->data.scope_id = get_scope_id(scp_s);

        b_tree_data data;
        data.initialized = true;
        data.return_type = r_type;
        data.symbol_type = SYM_CONST;
        data.scope_id = d_string(null_cond->data.scope_id);
        int ret;
        ret = declare_symbol(symtable_stack, null_cond->data.id_name, data);
        if (ret == INSERT_ALREADY_IN)
            return SEM_VARIABLE_ERR;
        if (ret == INSERT_MEM_ERR)
            return INTERNAL_ERR;
    }

    return NO_ERR;
}

error_code unary_operation(node* ast) {
    // Syntactically assured to not be NULL
    node* child = ast->children[0];
    data_type type = child->data.ret_value;

    if (type != I32 && type != F64) {
        return SEM_COMPATIBILITY_ERR;
    }

    return NO_ERR;
}

error_code return_statement(node* ast) {
    node* exp = get_child_by_type(ast, OPT_EXPR, 0);
    data_type func_rv = get_ancestor_by_type(ast, FUNC_DEF, 0)->data.ret_value;
    // Void return
    if (exp == NULL) {
        ast->data.ret_value = VOID;
    } else {
        ast->data.ret_value = exp->data.ret_value;
    }

    if (exp == NULL)
        return NO_ERR;

    // RETURN ERR analysation

    if (exp->child_count > 0) {
        if (func_rv == VOID) {
            return SEM_RETURN_ERR;
        }
    } else {
        if (func_rv != VOID) {
            return SEM_RETURN_ERR;
        }
    }

    return NO_ERR;
}

// Given a node and a type, checks whether all the return statements
// are reachable and whether they are of a compatible type
error_code check_returns(node* ast, data_type d) {
    if (ast->data.type == FUNC_DEF) {
        DEBUG_PRINT("Checking returns for %s", ast->data.id_name);
        return check_returns(get_child_by_type(ast, STMT_LIST, 0), d);
    }
    DEBUG_PRINT("Return checking: %s", NODE_TYPE_STRINGS[ast->data.type]);

    error_code er_code;
    bool ret_present = false;
    if (ast->data.type == RETURN) {
        DEBUG_PRINT("Found return statement of type %s, should be %s",
                    DATA_TYPE_STRINGS[ast->data.ret_value],
                    DATA_TYPE_STRINGS[d])
        if (ast->data.ret_value == d) {
            return NO_ERR;
        }
        return SEM_FUNCTION_ERR;
    }
    if (ast->data.type == STMT_LIST) {
        // Check for if statements
        for (int i = 0; i < ast->child_count; i++) {
            node* if_stmt = get_child_by_type(ast, IF, i);
            if (if_stmt == NULL) {
                break;
            }

            er_code = check_returns(if_stmt, d);

            if (er_code == SEM_FUNCTION_ERR) {
                return er_code;
            }
            if (er_code == NO_ERR) {
                ret_present = true;
            }
        }

        // Check for while-else statements
        for (int i = 0; i < ast->child_count; i++) {
            node* while_else = get_child_by_type(ast, WHILE_ELSE, i);
            if (while_else == NULL) {
                break;
            }

            er_code = check_returns(while_else, d);

            if (er_code == SEM_FUNCTION_ERR) {
                return er_code;
            }
            if (er_code == NO_ERR) {
                ret_present = true;
            }
        }

        // Check for return Statements
        for (int i = 0; i < ast->child_count; i++) {
            node* ret = get_child_by_type(ast, RETURN, i);
            if (ret == NULL) {
                break;
            }

            er_code = check_returns(ret, d);
            if (er_code == SEM_FUNCTION_ERR) {
                return er_code;
            }
            if (er_code == NO_ERR) {
                ret_present = true;
            }
        }

        if (ret_present || d == VOID) {
            return NO_ERR;
        }
        return SEM_RETURN_ERR;
    }

    if (ast->data.type == IF) {
        bool if_returns = false;
        node* if_stmt_list = get_child_by_type(ast, STMT_LIST, 0);
        node* else_stmt = get_child_by_type(ast, ELSE, 0);
        er_code = check_returns(if_stmt_list, d);

        if (er_code == SEM_FUNCTION_ERR) {
            return er_code;
        }
        if (er_code == NO_ERR) {
            if_returns = true;
        }

        // Check the else branch
        if (else_stmt != NULL) {
            er_code = check_returns(get_child_by_type(ast, STMT_LIST, 0), d);
            if (er_code == SEM_FUNCTION_ERR) {
                return er_code;
            }
            // Both branches return
            if (er_code == NO_ERR && if_returns) {
                return NO_ERR;
            }
            // One or both of the branches don't return
            return SEM_RETURN_ERR;
        }
        // If might contain return but without else it doesn't mean it is
        // reachable
        return SEM_RETURN_ERR;
    }

    if (ast->data.type == WHILE_ELSE) {
        bool while_returns = false;
        node* while_stmt_list = get_child_by_type(ast, STMT_LIST, 0);
        node* else_stmt = get_child_by_type(ast, ELSE, 0);
        er_code = check_returns(while_stmt_list, d);

        if (er_code == SEM_FUNCTION_ERR) {
            return er_code;
        }
        if (er_code == NO_ERR) {
            while_returns = true;
        }

        // Check the else branch
        if (else_stmt != NULL) {
            er_code = check_returns(get_child_by_type(ast, STMT_LIST, 0), d);
            if (er_code == SEM_FUNCTION_ERR) {
                return SEM_FUNCTION_ERR;
            }
            // Both branches return
            if (er_code == NO_ERR && while_returns) {
                return NO_ERR;
            }
            // One or both of the branches don't return
            return SEM_RETURN_ERR;
        }
        // while might contain return but without else it doesn't mean it is
        // reachable.
        return SEM_RETURN_ERR;
    }

    // we shouldn't ever get here
    ERROR_PRINT("check_returns is getting where it shouldn't%s", "");
    return SEM_RETURN_ERR;
}

error_code declare_function_signatures(node* ast, vector* symtable_stack) {
    DEBUG_PRINT("Declaring functions %s", "");

    node* f_def;

    // Iterate over all the children of this node.
    for (int i = 0; i < ast->child_count; i++) {
        f_def = ast->children[i];

        // If the node is not function definition skip it.
        if (f_def->data.type != FUNC_DEF)
            continue;

        // Set basic symbol information.
        b_tree_data data;
        data.symbol_type = SYM_FUNC;
        data.return_type = f_def->data.ret_value;

        node* param_list = get_child_by_type(f_def, PARAM_LIST, 0);

        // If function has parameters note them down in the symtable.
        if (param_list == NULL) {
            // No parameters
            data.func_parameters = NULL;
        } else {
            vector* params = vec_init(3, sizeof(data_type));
            if (params == NULL) {
                return INTERNAL_ERR;
            }
            for (int i = 0; i < param_list->child_count; i++) {
                node* param = get_child_by_type(param_list, PARAM, i);

                if (param == NULL) {
                    break;
                }
                data_type* d =
                    (data_type*)vec_push(params, &(param->data.ret_value));
                DEBUG_PRINT("Adding param %s to %s", DATA_TYPE_STRINGS[*d],
                            f_def->data.id_name);
                if (d == NULL) {
                    vec_free(&params);
                    return INTERNAL_ERR;
                }
            }
            if (params->len == 0) {
                vec_free(&params);
            }
            data.func_parameters = params;
        }

        int res = declare_symbol(symtable_stack, f_def->data.id_name, data);
        if (res == INSERT_ALREADY_IN) {
            return SEM_VARIABLE_ERR;
        }
        if (res == INSERT_MEM_ERR) {
            return INTERNAL_ERR;
        }
    }
    return NO_ERR;
}

error_code function_declaration(node* ast) {

    error_code err;
    // Check the validity of return statements inside the function.
    err = check_returns(ast, ast->data.ret_value);
    if (err != 0) {
        ERROR_PRINT("%s has invalid return(s)", ast->data.id_name);
        return err;
    }

    return NO_ERR;
}

// Adds scope id to whatever is passed to it.
error_code scoped_def(node* ast, scope_stack* scp_s) {
    ast->data.scope_id = get_scope_id(scp_s);
    if (ast->data.scope_id == NULL) {
        return INTERNAL_ERR;
    }
    return NO_ERR;
}

// Sets the return type of the parameter to that of the expression that is his
// child.
error_code input_param(node* ast) {
    node* exp = get_child_by_type(ast, EXPR, 0);
    ast->data.ret_value = exp->data.ret_value;
    return NO_ERR;
}

error_code opt_expr(node* ast) {
    node* exp = get_child_by_type(ast, EXPR, 0);
    if (exp != NULL) {
        ast->data.ret_value = exp->data.ret_value;
        DEBUG_PRINT("Optexpr, type is %s",
                    DATA_TYPE_STRINGS[ast->data.ret_value]);
    }
    return NO_ERR;
}

error_code ternary(node* ast) {
    node* exp0 = get_child_by_type(ast, EXPR, 0);
    node* exp1 = get_child_by_type(ast, EXPR, 1);

    if (exp0->data.ret_value == exp1->data.ret_value) {
        ast->data.ret_value = exp0->data.ret_value;
        return NO_ERR;
    }

    return SEM_COMPATIBILITY_ERR;
}

error_code orelse(node* ast) {
    node* left = ast->children[0];
    node* right = ast->children[1];

    if (type_is_optional(left->data.ret_value)) {
        if (unwrap_optional(left->data.ret_value) == right->data.ret_value ||
            left->data.ret_value == right->data.ret_value) {
            ast->data.ret_value = right->data.ret_value;
            return NO_ERR;
        }
        if (right->data.type == UNREACHABLE) {
            if (type_is_optional(left->data.ret_value)) {
                ast->data.ret_value = unwrap_optional(left->data.ret_value);
            } else {
                ast->data.ret_value = left->data.ret_value;
            }
            return NO_ERR;
        }
        if (right->data.ret_value == DT_NULL) {
            ast->data.ret_value = left->data.ret_value;
            return NO_ERR;
        }
    }

    return SEM_COMPATIBILITY_ERR;
}

error_code semantically_analyse(node* ast, vector* symtable_stack,
                                scope_stack* scp_s) {

    DEBUG_PRINT("Semantically analysing %s", NODE_TYPE_STRINGS[ast->data.type]);

    node_type type = ast->data.type;
    error_code return_code;
    // The nodes at which a new scope should be entered
    bool scoped =
        type == FUNC_DEF || type == WHILE || type == IF || type == FOR;

    // Else is nested inside 'IF' node so we need to pop the IF scope first.
    if (type == ELSE) {
        DEBUG_PRINT("Leaving scope - IF%s", "");
        return_code = leave_scope(symtable_stack);
        if (return_code != NO_ERR) {
            return return_code;
        }
        pop_scope(scp_s);
    }
    // Else enters the scope here but leaves the scope with the 'IF' since
    // it's nested.
    if (scoped || type == ELSE) {
        DEBUG_PRINT("Entering scope - %s", NODE_TYPE_STRINGS[type]);
        return_code = enter_scope(symtable_stack);
        if (return_code != 0) {
            ERROR_PRINT("Semantic error: Failed to enter scope - %s",
                        ERROR_STRINGS[return_code]);
        }
        push_scope(scp_s);
    }

    // Initialize the scope stack that will be used to add scope id to
    // variables inside different scopes in the function.
    if (ast->data.type == FUNC_DEF) {
        scp_s = scope_stack_init(4);
    }

    if (ast->data.type == PROG) {
        // Prioritly declare function signatures
        return_code = declare_function_signatures(ast, symtable_stack);
        if (return_code != NO_ERR)
            return return_code;
        // Missing main
        b_tree_data* main_data = find_symbol(symtable_stack, "main");
        if (main_data == NULL) {
            ERROR_PRINT("Main is not declared %s", "");
            return SEM_UNDEFINED_ERR;
        }
        if (main_data == NULL || main_data->return_type != VOID ||
            main_data->func_parameters != NULL) {
            ERROR_PRINT("Main is declared with incorrect return type or "
                        "incorrect parameters%s",
                        "");
            return SEM_FUNCTION_ERR;
        }
    }

    // Recursively analyse the children of this node.
    for (int i = 0; i < ast->child_count; i++) {

        return_code =
            semantically_analyse(ast->children[i], symtable_stack, scp_s);
        if (return_code != 0) {
            ERROR_PRINT("Semantic error: %s - %s",
                        NODE_TYPE_STRINGS[ast->children[i]->data.type],
                        ERROR_STRINGS[return_code]);
            return return_code;
        }

        DEBUG_PRINT("Done analysing %s", NODE_TYPE_STRINGS[ast->data.type]);
    }

    error_code err = NO_ERR;

    // Check this node based on what it is.
    switch (ast->data.type) {
        // Unused enum members listed for overview.
    case UNREACHABLE:
    case NULL_LIT:
    case STMT_LIST:
    case NULL_COND:
    case WHILE_CNT:
    case WHILE_ELSE:
    case INPUT_PARAM_LIST:
    case IS_UNREACHABLE:
    case LITERAL:
    case ERR:
    case PROG:
    case PARAM_LIST:
    case ELSE:
    case TYPE:
    case AS_FUNC:
        // Nothing to check
        break;
    case ORELSE:
        err = orelse(ast);
        break;
    case IF_TERNARY:
        err = ternary(ast);
        break;
    case OPT_EXPR:
        err = opt_expr(ast);
        break;
    case INPUT_PARAM:
        err = input_param(ast);
        break;
    case FUNC_DEF:
        err = function_declaration(ast);
        break;
    case RETURN:
        err = return_statement(ast);
        break;
    case BREAK:
    case CONTINUE:
        err = check_label(ast);
        break;
    case INT_NEGATE:
    case B_NEG:
        err = unary_operation(ast);
        break;
    case VAR_DEF:
    case CONST_DEF:
        err = declaration(ast, symtable_stack, scp_s);
        break;
    case VAR:
        err = var(ast, symtable_stack, scp_s);
        break;
    case FUNC:
        err = function_call(ast, symtable_stack);
        break;
    case COND:
        err = cond(ast, symtable_stack, scp_s);
        break;
    case WHILE:
        err = while_def(ast, scp_s);
        break;
    case FOR:
    case IF:
        err = scoped_def(ast, scp_s);
        break;
    case PARAM:
        err = param(ast, symtable_stack, scp_s);
        break;
    case EXPR:
        err = expression(ast);
        break;
    case ASSIGN_THROW_AWAY:
    case ASSIGN:
        err = assignment(ast, symtable_stack, scp_s);
        break;
    // Bitwise operators
    case B_AND:
    case B_OR:
        err = bitwise_operators(ast, symtable_stack);
        break;
    // Arithmetic operators
    case MUL:
    case DIV:
    case ADD:
    case SUB:
        err = arith_operators(ast, symtable_stack);
        break;
    case EQUAL:
    case NOT_EQUAL:
    case EQUALMORE:
    case EQUALLESS:
    case MORE:
    case LESS:
        err = logic_operators(ast, symtable_stack);
        break;
    }

    if (scoped) {
        DEBUG_PRINT("Leaving scope - %s", NODE_TYPE_STRINGS[type]);
        return_code = leave_scope(symtable_stack);
        if (return_code != NO_ERR) {
            return return_code;
        }
        pop_scope(scp_s);
    }

    if (ast->data.type == FUNC_DEF) {
        scope_stack_free(&scp_s);
    }

    return err;
}
