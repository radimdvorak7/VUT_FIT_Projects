// Projekt: Implementace přeladače jazyka IFJ24
// Autor: Dominik Václavík (xvacla37)

#include "code_gen.h"

#include "code_gen_help.h"
#include "data_types.h"
#include "func_look_up.h"

#include <assert.h>
#include <stdio.h>
#include <string.h>

#define fptr stdout

void handle_func_def(node* func_def);
void handle_stmt_list(node* stmt_list);
void handle_func_def_var(node* stmt_list);
void handle_statement(node* statement);

void handle_statement_return(node* ret);
void handle_statement_function(node* function);
void handle_assign_throw_away_function(node* throw);
void handle_variable_definition(node* variable);
void handle_assign(node* assing);
void handle_if_statement(node* if_statement);
void handle_while_statement(node* while_statement);
void handle_continue_statement(node* continue_statement);
void handle_break_statement(node* break_statement);

void handle_expression(node* expression);

void handle_expression_rec(node* child);
void handle_null_literal(node* literal);
void handle_literal(node* literal);
void handle_var(node* var);
void handle_operand(node* operand);
void handle_function(node* function);

fl_tree function_look_up;
l_data label_data = {0};
l_stack* label_stack = NULL;
char* function_name_ref = NULL;
error_code generate_code(node* ast_tree) {
    print_tree(ast_tree);

    error_code ret_code = NO_ERR;

    fl_tree_init(&function_look_up);
    ret_code = fl_tree_insert_functions(&function_look_up);
    if(ret_code != NO_ERR){
        fl_tree_free(&function_look_up);
        return ret_code;
    }

    // init label stack
    label_stack = l_stack_init();
    if(label_stack == NULL){
        fl_tree_free(&function_look_up);
        return INTERNAL_ERR;
    }

    // assert PROG
    assert(ast_tree != NULL);
    assert(ast_tree->data.type == PROG);

    // Create prolog
    fprintf(fptr, ".IFJcode24\n"
                  "DEFVAR GF@null\n"
                  "CALL main\n"
                  "EXIT int@0\n"
                  "\n");

    for (int i = 0; i < ast_tree->child_count; i++) {
        node* function_node = ast_tree->children[i];

        // assert FUNC_DEF
        assert(function_node != NULL);
        assert(function_node->data.type == FUNC_DEF);

        handle_func_def(function_node);
    }

    fl_tree_include(&function_look_up, fptr);

    l_stack_free(&label_stack);
    fl_tree_free(&function_look_up);

    return ret_code;
}

void handle_func_def(node* func_def) {
    assert(func_def->child_count == 2);

    fprintf(fptr,
            "LABEL %s\n"
            "CREATEFRAME\n",
            func_def->data.id_name);

    function_name_ref = func_def->data.id_name;

    node* param_list = func_def->children[0];
    assert(param_list != NULL);
    assert(param_list->data.type == PARAM_LIST);

    // Handle PARAM_LIST
    for (int i = param_list->child_count - 1; i >= 0; i--) {
        node* param = param_list->children[i];

        assert(param != NULL);
        assert(param->data.type == PARAM);

        fprintf(fptr,
                "DEFVAR TF@%s%s\n"
                "POPS TF@%s%s\n",
                param->data.scope_id, param->data.id_name, param->data.scope_id,
                param->data.id_name);
    }

    node* stmt_list = func_def->children[1];
    handle_func_def_var(stmt_list);

    fprintf(fptr, "PUSHFRAME\n");

    // Handle Statement list
    handle_stmt_list(stmt_list);

    // Generate return statement if there is no return at the end
    if ((stmt_list->child_count == 0) || (stmt_list->children[stmt_list->child_count - 1]->data.type != RETURN)) {
        fprintf(fptr, "POPFRAME\n"
                      "RETURN\n"
                      "\n");
    } else {
        fprintf(fptr, "\n");
    }
}

void handle_func_def_var(node* stmt_list) {
    for (int i = 0; i < stmt_list->child_count; i++) {
        node* child_node = stmt_list->children[i];

        switch (child_node->data.type) {
        case VAR_DEF:
        case CONST_DEF:
        case NULL_COND:
            fprintf(fptr, "DEFVAR TF@%s%s\n", child_node->data.scope_id,
                    child_node->data.id_name);
            break;
        default:
            handle_func_def_var(child_node);
            break;
        }
    }
}

void handle_stmt_list(node* stmt_list) {
    assert(stmt_list != NULL);
    assert(stmt_list->data.type == STMT_LIST);

    // Handle STMT_LIST
    for (int i = 0; i < stmt_list->child_count; i++) {
        node* statement = stmt_list->children[i];

        assert(statement != NULL);

        handle_statement(statement);
    }
}

void handle_statement(node* statement) {
    switch (statement->data.type) {
    case RETURN:
        handle_statement_return(statement);
        break;
    case FUNC:
        handle_statement_function(statement);
        break;
    case ASSIGN_THROW_AWAY:
        handle_assign_throw_away_function(statement);
        break;
    case VAR_DEF:
    case CONST_DEF:
        handle_variable_definition(statement);
        break;
    case ASSIGN:
        handle_assign(statement);
        break;
    case IF:
        handle_if_statement(statement);
        break;
    case WHILE:
        handle_while_statement(statement);
        break;
    case BREAK:
        handle_break_statement(statement);
        break;
    case CONTINUE:
        handle_continue_statement(statement);
        break;
    default:
        fprintf(stderr, "\nUknown node type: %s\n",
                get_type_str(statement->data.type));
        print_tree(statement);
        fprintf(stderr, "\n");
        break;
    }
}

void handle_statement_return(node* ret) {
    assert(ret->child_count == 1);

    node* opt_exp = ret->children[0];
    assert(opt_exp != NULL);
    assert(opt_exp->data.type == OPT_EXPR);

    if (opt_exp->child_count == 1) {
        node* exp = opt_exp->children[0];
        assert(exp != NULL);
        assert(exp->data.type == EXPR);

        handle_expression(exp);
    } else {
        assert(opt_exp->child_count == 0);
    }

    fprintf(fptr, "POPFRAME\n"
                  "RETURN\n");
}

void handle_statement_function(node* function) {
    handle_function(function);
    if (function->data.ret_value != VOID) {
        fprintf(fptr, "POPS GF@null\n");
    }
}

void handle_variable_definition(node* variable) {
    assert(variable != NULL);
    assert(variable->data.type == VAR_DEF || variable->data.type == CONST_DEF);

    // fprintf(fptr,
    //     "DEFVAR LF@%s%s\n"
    //, variable->data.scope_id, variable->data.id_name);

    if (variable->child_count == 1) {
        node* expression = variable->children[0];

        handle_expression(expression);

        fprintf(fptr, "POPS LF@%s%s\n", variable->data.scope_id,
                variable->data.id_name);
    } else {
        assert(variable->child_count == 0);
    }
}

void handle_assign(node* assing) {
    assert(assing != NULL);
    assert(assing->data.type == ASSIGN);
    assert(assing->child_count == 1);

    node* expression = assing->children[0];
    handle_expression(expression);

    fprintf(fptr, "POPS LF@%s%s\n", assing->data.scope_id,
            assing->data.id_name);
}

void handle_assign_throw_away_function(node* throw) {
    assert(throw->child_count == 1);
    node* expr = throw->children[0];
    assert(expr->data.type == EXPR);

    handle_expression(expr);

    fprintf(fptr, "POPS GF@null\n");
}

void handle_if_statement(node* if_statement) {
    assert(if_statement != NULL);
    assert(if_statement->data.type == IF);

    assert(if_statement->child_count > 1);
    node* cond = if_statement->children[0];
    assert(cond->data.type == COND);

    assert(cond->child_count > 0);
    node* expr = cond->children[0];
    handle_expression(expr);

    bool have_null = false;
    if (cond->child_count == 2) {
        have_null = true;
        node* null_cond = cond->children[1];

        fprintf(fptr,
                "POPS LF@%s%s\n"
                "PUSHS LF@%s%s\n",
                null_cond->data.scope_id, null_cond->data.id_name,
                null_cond->data.scope_id, null_cond->data.id_name);
    } else {
        assert(cond->child_count == 1);
    }

    node* if_statement_block = if_statement->children[1];
    assert(if_statement_block->data.type == STMT_LIST);

    if (if_statement->child_count == 2) {
        if (have_null) {
            fprintf(fptr,
                    "PUSHS nil@nil\n"
                    "JUMPIFEQS $%s%sifend\n",
                    function_name_ref, if_statement->data.scope_id);
        } else {
            fprintf(fptr,
                    "PUSHS bool@false\n"
                    "JUMPIFEQS $%s%sifend\n",
                    function_name_ref, if_statement->data.scope_id);
        }

        // HANDLE THE BLOCK
        handle_stmt_list(if_statement_block);

        fprintf(fptr, "LABEL $%s%sifend\n", function_name_ref,
                if_statement->data.scope_id);
    } else {
        assert(if_statement->child_count == 3);
        node* else_block = if_statement->children[2];
        assert(else_block->data.type == ELSE);
        assert(else_block->child_count == 1);

        if (have_null) {
            fprintf(fptr,
                    "PUSHS nil@nil\n"
                    "JUMPIFEQS $%s%sifelse\n",
                    function_name_ref, if_statement->data.scope_id);
        } else {
            fprintf(fptr,
                    "PUSHS bool@false\n"
                    "JUMPIFEQS $%s%sifelse\n",
                    function_name_ref, if_statement->data.scope_id);
        }

        // HANDLE THE BLOCK
        handle_stmt_list(if_statement_block);

        fprintf(fptr,
                "JUMP $%s%sifend\n"
                "LABEL $%s%sifelse\n",
                function_name_ref, if_statement->data.scope_id,
                function_name_ref, if_statement->data.scope_id);

        node* else_stmt_block = else_block->children[0];
        assert(else_stmt_block->data.type == STMT_LIST);
        // HANDLE THE BLOCK
        handle_stmt_list(else_stmt_block);

        fprintf(fptr, "LABEL $%s%sifend\n", function_name_ref,
                if_statement->data.scope_id);
    }
}

void handle_while_statement(node* while_statement) {
    assert(while_statement != NULL);
    assert(while_statement->data.type == WHILE);

    assert(while_statement->child_count > 1);
    node* cond = get_child_by_type(while_statement, COND, 0);
    assert(cond != NULL);

    node* stmt = get_child_by_type(while_statement, STMT_LIST, 0);
    assert(stmt != NULL);

    node* while_cnt = get_child_by_type(while_statement, WHILE_CNT, 0);
    node* while_else = get_child_by_type(while_statement, WHILE_ELSE, 0);

    assert(cond->child_count > 0);
    node* expression = cond->children[0];
    node* nullcond = get_child_by_type(cond, NULL_COND, 0);

    char* label = while_statement->data.label;
    if (label == NULL) {
        label = "WHILE";
    }

    label_data.name = label;
    label_data.scope = while_statement->data.scope_id;
    l_stack_push(label_stack, &label_data);

    // if(nullcond != NULL){
    //     fprintf(fptr,
    //         "DEFVAR LF@%s%s\n"
    //     , nullcond->data.scope_id, nullcond->data.id_name);
    // }
    fprintf(fptr, "LABEL $%s%s%s$while\n", function_name_ref, label_data.scope,
            label_data.name);

    handle_expression(expression);

    if (nullcond != NULL) {
        fprintf(fptr,
                "POPS LF@%s%s\n"
                "PUSHS LF@%s%s\n",
                nullcond->data.scope_id, nullcond->data.id_name,
                nullcond->data.scope_id, nullcond->data.id_name);
    }

    if (nullcond != NULL) {
        fprintf(fptr, "PUSHS nil@nil\n");
    } else {
        fprintf(fptr, "PUSHS bool@false\n");
    }
    fprintf(fptr, "JUMPIFEQS $%s%s%s$end\n", function_name_ref,
            label_data.scope, label_data.name);

    handle_stmt_list(stmt);

    fprintf(fptr, "LABEL $%s%s%s$continue\n", function_name_ref,
            label_data.scope, label_data.name);

    if (while_cnt != NULL) {
        assert(while_cnt->child_count == 1);
        node* while_cnt_stmt = while_cnt->children[0];

        handle_stmt_list(while_cnt_stmt);
    }

    fprintf(fptr, "JUMP $%s%s%s$while\n", function_name_ref, label_data.scope,
            label_data.name);

    fprintf(fptr, "LABEL $%s%s%s$end\n", function_name_ref, label_data.scope,
            label_data.name);

    if (while_else != NULL) {
        assert(while_else->child_count == 1);
        node* while_else_stmt = while_else->children[0];

        handle_stmt_list(while_else_stmt);
    }

    fprintf(fptr, "LABEL $%s%s%s$break\n", function_name_ref, label_data.scope,
            label_data.name);

    l_stack_pop(label_stack, &label_data);
}

void handle_break_statement(node* break_statement) {
    assert(break_statement != NULL);
    assert(break_statement->child_count == 0);
    char * break_scope;
    char * break_label;
    if(break_statement->data.label == NULL){
        break_scope = label_data.scope;
        break_label = label_data.name;
    }
    else {
        break_scope = break_statement->data.scope_id;
        break_label = break_statement->data.label;
    }
    fprintf(fptr, "JUMP $%s%s%s$break\n", function_name_ref, break_scope,
            break_label);
}

void handle_continue_statement(node* continue_statement) {
    assert(continue_statement != NULL);
    assert(continue_statement->child_count == 0);
    char * continue_scope;
    char * continue_label;
    if(continue_statement->data.label == NULL){
        continue_scope = label_data.scope;
        continue_label = label_data.name;
    }
    else {
        continue_scope = continue_statement->data.scope_id;
        continue_label = continue_statement->data.label;
    }
    fprintf(fptr, "JUMP $%s%s%s$continue\n", function_name_ref,
            continue_scope, continue_label);
}

void handle_expression(node* expression) {
    assert(expression != NULL);
    assert(expression->data.type == EXPR);
    assert(expression->child_count == 1);

    node* child = expression->children[0];
    assert(child != NULL);

    handle_expression_rec(child);
}

void handle_expression_rec(node* child) {
    switch (child->data.type) {
    case LITERAL:
        handle_literal(child);
        break;
    case NULL_LIT:
        handle_null_literal(child);
        break;
    case VAR:
        handle_var(child);
        break;
    case FUNC:
        handle_function(child);
        break;
    case EXPR:
        handle_expression(child);
        break;
    case MUL:
    case DIV:
    case ADD:
    case SUB:
    case ORELSE:
    case INT_NEGATE:
    case EQUAL:
    case NOT_EQUAL:
    case EQUALMORE:
    case EQUALLESS:
    case MORE:
    case LESS:
    case B_AND:
    case B_OR:
    case B_NEG:
        handle_operand(child);
        break;
    default:
        fprintf(stderr, "\nUknown node type: %s\n",
                get_type_str(child->data.type));
        print_tree(child);
        fprintf(stderr, "\n");
        break;
    }
}

void handle_null_literal(node* literal) {
    assert(literal->child_count == 0);

    fprintf(fptr, "PUSHS nil@nil\n");
}

void handle_literal(node* literal) {
    assert(literal->child_count == 0);

    switch (literal->data.ret_value) {
    case STRING_LITERAL:
    case U8_SLICE:
        fprintf(fptr, "PUSHS string@");
        print_string_format(fptr, literal->data.str_value);
        fprintf(fptr, "\n");
        break;
    case I32:
        fprintf(fptr, "PUSHS int@%d\n", literal->data.int_value);
        break;
    case F64:
        fprintf(fptr, "PUSHS float@%a\n", literal->data.float_value);
        break;
    case DT_NULL:
        fprintf(fptr, "PUSHS nil@nil\n");
        break;
    case BOOLEAN:
        fprintf(fptr, "PUSHS bool@%s\n",
                literal->data.bool_value ? "true" : "false");
        break;
    default:
        fprintf(stderr, "\nUknown node return type: %d\n",
                literal->data.ret_value);
        break;
    }
}

void handle_var(node* var) {
    assert(var->child_count == 0);
    assert(var->data.type == VAR);

    fprintf(fptr, "PUSHS LF@%s%s\n", var->data.scope_id, var->data.id_name);
}

void handle_operand(node* operand) {
    switch (operand->data.type) {
    case MUL:
    case DIV:
    case ADD:
    case SUB:
    case EQUAL:
    case NOT_EQUAL:
    case EQUALMORE:
    case EQUALLESS:
    case MORE:
    case LESS:
    case B_AND:
    case B_OR:
        assert(operand->child_count == 2);
        handle_expression_rec(operand->children[0]);
        handle_expression_rec(operand->children[1]);
        break;
    case INT_NEGATE:
    case B_NEG:
        assert(operand->child_count == 1);
        handle_expression_rec(operand->children[0]);
        break;
    case ORELSE:
        assert(operand->child_count == 2);
        handle_expression_rec(operand->children[0]);
        if(operand->children[1]->data.type != UNREACHABLE){
            handle_expression_rec(operand->children[1]);
        }
        break;
    default:
        break;
    }
    switch (operand->data.type) {
    case MUL:
        fprintf(fptr, "MULS\n");
        break;
    case DIV:
        if (operand->data.ret_value == I32) {
            fprintf(fptr, "IDIVS\n");
        } else if (operand->data.ret_value == F64) {
            fprintf(fptr, "DIVS\n");
        } else {
            fprintf(stderr, "Uknow type in DIV\n");
            assert(false);
        }
        break;
    case ADD:
        fprintf(fptr, "ADDS\n");
        break;
    case SUB:
        fprintf(fptr, "SUBS\n");
        break;
    case ORELSE:
        if(operand->children[1]->data.type == UNREACHABLE){
            fprintf(fptr, "CALL %s\n",
                fl_tree_use(&function_look_up, "bld.orelse_unreachable"));
        }
        else {
            fprintf(fptr, "CALL %s\n",
                fl_tree_use(&function_look_up, "bld.orelse"));
        }
        break;
    case EQUAL:
        fprintf(fptr, "EQS\n");
        break;
    case NOT_EQUAL:
        fprintf(fptr, "EQS\nNOTS\n");
        break;
    case EQUALMORE:
        fprintf(fptr, "LTS\n"
                      "NOTS\n");
        break;
    case EQUALLESS:
        fprintf(fptr, "GTS\n"
                      "NOTS\n");
        break;
    case MORE:
        fprintf(fptr, "GTS\n");
        break;
    case LESS:
        fprintf(fptr, "LTS\n");
        break;
    case B_AND:
        fprintf(fptr, "ANDS\n");
        break;
    case B_OR:
        fprintf(fptr, "ORS\n");
        break;
    case INT_NEGATE:
        fprintf(fptr, "PUSHS int@-1\n"
                      "MULS\n");
        break;
    case B_NEG:
        fprintf(fptr, "NOTS\n");
        break;
    default:
        fprintf(stderr, "\nUknown node type: %s\n",
                get_type_str(operand->data.type));
        print_tree(operand);
        fprintf(stderr, "\n");
        break;
    }
}

void handle_function(node* function) {
    assert(function != NULL);
    assert(function->data.type == FUNC);
    assert(function->child_count == 1);

    node* params = function->children[0];
    assert(params != NULL);
    assert(params->data.type == INPUT_PARAM_LIST);

    for (int i = 0; i < params->child_count; i++) {
        node* param = params->children[i];
        assert(param != NULL);
        assert(param->data.type == INPUT_PARAM);
        assert(param->child_count == 1);

        node* expr = param->children[0];
        handle_expression(expr);
    }

    if (strncmp(function->data.id_name, "ifj.", 4) == 0) {
        fprintf(fptr, "CALL %s\n",
                fl_tree_use(&function_look_up, function->data.id_name));
    } else {
        fprintf(fptr, "CALL %s\n", function->data.id_name);
    }
}

