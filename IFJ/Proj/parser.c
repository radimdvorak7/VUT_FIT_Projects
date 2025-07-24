// Project: Implementation of a compiler for the IFJ24 imperative language.
// Author: František Sedláček ( xsedla2d )
//
// Main file of Syntactic parser, where recursive descent is utilized with LL rules.


#include "parser.h"
#include <stdbool.h>
#include <stdlib.h>
#include <string.h> // I hate this
#include "scanner.h"
#include "data_types.h"

// GLOBAL
// Note: instead of passing down a pointer to an error, error number will be stored here.
// Note: This variable will stay. it is used when getting new tokens or as it gets a lexical error.
int g_err=0;
// since all states return an expression comprised of "and" operands, one false exit in evaluating
// the previous return statement guarantees not going into another state by the compiler.

#ifndef NDEBUG
#define NODE_CPY_TOKEN_PRINT \
        fprintf(stderr, "\x1b[2m" "%s: expected Identifier token, %s. Value copied: %s" "\x1b[2m" "\n", \
        __func__,t->type == 1000?"got one":"not identifier",t->type == 1000?t->content.id:"NOT COPIED");
#define NODE_CPY_T_PRINT_EXTENDED \
        fprintf(stderr, "\x1b[2m" "%s: extended to %s"  "\x1b[2m" "\n", \
        __func__,dest_n->data.id_name);
#else
#define NODE_CPY_TOKEN_PRINT
#define NODE_CPY_T_PRINT_EXTENDED
#endif

/**
 *  Method allocates memory for identification a node and copies the string into the node.
 * @param in_str input string
 * @param dest_n destination node
 * @return true, if copied successfully
 */
bool node_cpy_id(char* in_str ,node* dest_n){
    char* node_id = malloc(sizeof(char)*(strlen(in_str)+1));
    if(node_id == NULL){
        g_err=99;
        return false;
    }
    dest_n->data.id_name = node_id;
    strcpy(node_id,in_str);
    return true;
}

/**
 * Method is similar to "apt_t", however identification is copied onto the node tree when successful.
 * passed token must be of type TOKEN_IDENT. Returned false if this criteria is not met.
 * @param t Token
 * @param dest_n Destination node
 * @return true, when successful.
 */
bool apt_t_cpy_id(token* const t, node *dest_n, bool namespace_allowed){
    // accept token
    NODE_CPY_TOKEN_PRINT
    if(t->type != TOKEN_IDENT)return false;
    // copy id into the node.
    node_cpy_id(t->content.id,dest_n);
    g_err = get_token(t);
    if(t->type != TOKEN_DOT || !namespace_allowed) return !g_err;

    // check if said id was namespace and concatenate.
    // Note: ifj code does not support objects.
    g_err = get_token(t);
    if(g_err || t->type != TOKEN_IDENT) return false;
    void* mem;
    mem = realloc(dest_n->data.id_name, strlen(dest_n->data.id_name)+2+strlen(t->content.id)+1);
    if(!mem){
        g_err = INTERNAL_ERR;
        return false;
    }
    strcat(mem,".");
    strcat(mem,t->content.id);
    dest_n->data.id_name= mem;
    NODE_CPY_T_PRINT_EXTENDED
    g_err = get_token(t);
    return !g_err;
}

#ifndef NDEBUG
#define APT_TOKEN_PRINT \
        fprintf(stderr, "\x1b[2m" "apt_t : expected Token type %s, got %s, %s" "\x1b[2m" "\n",TOKEN_STRINGS[t_type], \
        TOKEN_STRINGS[t->type],t->type==t_type?"accepting":"\x1b[31m rejecting \x1b[0m" );
#else
#define APT_TOKEN_PRINT
#endif

/**
 * Method accepts a token when token types are the same.
 * When a token is accepted, it will be overwritten with another token from scanner.
 * @param t token
 * @param t_type token type to compare.
 * @return true when token types are the same.
 */
bool apt_t(token* const t, token_type t_type){
    // accept token
    APT_TOKEN_PRINT
    bool result = t->type == t_type;
    if(!result){
        g_err = SYNTACTIC_ERR;
        return false;
    }
    // EOF FOUND
    if(t->type == TOKEN_EOF) return result;
    // next token.
    g_err = get_token(t);
    return !g_err;
}


/**
 * Method accepts a semicolon token
 * OR skips when parental node of parent is a continue block.
 * NOTE: this function is a last resort because of incorrect LL(0) grammar,
 * which does not fit the bonus parameters. due to statements
 *
 * When a token is accepted, it will be overwritten with another token from scanner.
 * @param t token
 * @return true when semicolon is accepted OR
 */
bool apt_t_semicolon(token* const t, node* parent){
    if(has_parent_type(parent,WHILE_CNT)) return true;
    return apt_t(t,TOKEN_SEMICOLON);
}


bool has_namespace(const node* n){
    return strstr(n->data.id_name,".");
}


/*
 * Recursive descent functions.
 * These functions represent each non-terminal in LL rules.
 * For more clarity, to where the functions belong,
 * all have a prefix "s_" signifying that they are part of the syntactic parser descent methods
 * functions return false when met with an unexpected symbol/word.
 * parameters passed down are the pointer to the token and a parent node.
 */

bool s_expr(token* const t, node* p);


bool s_start(token* const t, node* syn_root);
bool s_prolog(token* const t);
bool s_func_list(token* const t, node* p);
bool s_func_def(token* const t, node* p);
bool s_param_list(token* const t, node* p);
bool s_ret_type(token* const t, node* p);
bool s_stmt_list(token* const t, node* p);
bool s_stmt(token* const t, node* p);
bool s_var_def_stmt(token* const t, node* p);
bool s_acl_stmt(token* const t, node* p);
bool s_param(token* const t, node* p);
bool s_p_sep(token* const t, node* p);
bool s_definition_type(token* const t, node *p);
bool s_var_con(token* const t, node *p);
bool s_type_and_assign(token* const t, node *p);
bool s_opt_type(token* const t, node* p);
bool s_if_stmt(token* const t, node* p);
bool s_while_stmt(token* const t, node* p);
bool s_func_call_stmt(token* const t, node* p);
bool s_ret_stmt(token* const t, node* p);
bool s_for_stmt(token* const t, node* p);
bool s_assign_stmt(token* const t, node* p);
bool s_cond(token* const t, node* p);
bool s_null_cond(token* const t, node* p);
bool s_if_block(token* const t, node* p);
bool s_else_block(token* const t, node* p);
bool s_else_part(token* const t, node* p);
bool s_input_param_list(token* const t, node* p);
bool s_input_param(token* const t, node *p);
bool s_ip_sep(token* const t, node *p);
bool s_for_block(token* const t, node* p);
bool s_spec_while_cnt(token* const t, node* p);
bool s_spec_while_if(token* const t, node* p);
bool s_spec_while_else(token* const t, node* p);
bool s_cnt_block(token* const t, node* p);
bool s_spec_while_else_block(token* const t, node* p);
bool s_continue_stmt(token* const t, node* p);
bool s_break_stmt(token* const t, node* p);


#ifndef NDEBUG
#define STATE_PRINT \
        fprintf(stderr, "\x1b[2m" "Entered state %s:" "\x1b[0m" "\n" ,__func__);
#define STATE_EXIT_PRINT \
        fprintf(stderr, "\x1b[2m" "Exited state %s successfully:" "\x1b[0m" "\n" ,__func__);
#else
#define STATE_PRINT
#endif

bool s_start(token* const t, node *syn_root){

    return s_prolog(t)
        && s_func_list(t, syn_root)
        && apt_t(t, TOKEN_EOF);
}

// basically accepts the IFJ prolog syntactically.
bool s_prolog(token* const t){
    STATE_PRINT
    // const
    bool result = apt_t(t, TOKEN_KW_CONST);
    // ifj
    if(t->type != TOKEN_IDENT)return false;
    if(strcmp(t->content.id,"ifj")!=0) return false;
    g_err = get_token(t);
    if(g_err) return false;
    // @import(
    result = result
            && apt_t(t, TOKEN_ASSIGN) &&
             apt_t(t, TOKEN_IMPORT) &&
             apt_t(t, TOKEN_L_PAREN);
    if(!result)return false;

    // "ifj24.zig"
    if(t->type != TOKEN_LIT_STRING)return false;
    if(strcmp(t->content.string_lit,"ifj24.zig")!=0)return false;
    g_err = get_token(t);
    if(g_err) return false;

    return apt_t(t, TOKEN_R_PAREN)
    && apt_t(t,TOKEN_SEMICOLON);
}

bool s_func_list(token* const t, node *p){
    STATE_PRINT
    switch (t->type) {
        case TOKEN_EOF:
            return true;
            // FUNC_LIST -> FUNC_DEF
        case TOKEN_KW_PUB:
            return s_func_def(t, p)
            && s_func_list(t, p);
        default:
            return false;
    }
}

bool s_func_def(token* const t, node *p){
    STATE_PRINT
    node *this = create_node(FUNC_DEF);
    append_node(this, p);
    return apt_t(t, TOKEN_KW_PUB) &&
           apt_t(t, TOKEN_KW_FN) &&
           apt_t_cpy_id(t, this, false) &&
           apt_t(t, TOKEN_L_PAREN) &&
           s_param_list(t, this) &&
           apt_t(t, TOKEN_R_PAREN) &&
           s_ret_type(t, this) &&
           apt_t(t, TOKEN_L_CURLY) &&
           s_stmt_list(t, this) &&
           apt_t(t, TOKEN_R_CURLY);
}

bool s_param_list(token* const t, node* p){
    STATE_PRINT
    node *n_this = create_node(PARAM_LIST);
    append_node(n_this,p);
    switch (t->type) {
        // PARAM_LIST -> e
        case TOKEN_R_PAREN:
            return true;
            // PARAM_LIST -> PARAM
        case TOKEN_IDENT:
            return s_param(t, n_this);
        default:
            return false;
    }
}

bool s_param(token* const t, node *p){
    STATE_PRINT
    // append parameter to list.
    node *this = create_node(PARAM);
    append_node(this, p);
    return apt_t_cpy_id(t, this, false) &&
           apt_t(t, TOKEN_COLON) &&
           s_definition_type(t, this) &&
            s_p_sep(t, p);

}

bool s_p_sep(token* const t, node* p){
    STATE_PRINT
    switch(t->type){
        // P_SEP    ->  , PARAM
        case TOKEN_COMMA:
            return apt_t(t,TOKEN_COMMA)
                && s_param(t,p);
            // P_SEP    ->  e
        case TOKEN_R_PAREN:
            return true;
        default:
            return false;
    }
}

bool s_ret_type(token* const t, node* p){
    STATE_PRINT
    switch (t->type) {
    // RET_TYPE ->  void
        case TOKEN_KW_VOID:
            apt_t(t,TOKEN_KW_VOID);
            p->data.ret_value = VOID;
            return true && !g_err;
    // RET_TYPE ->  D_TYPE
        default:
            return s_definition_type(t, p);
    }
}

bool s_stmt_list(token* const t, node* p){
    STATE_PRINT
    node* this;
    // due to recursive descent, this prevents a creation of a duplicate STMT_LIST node
    // Note: this prevention is also used for BLOCKS (for example WHILE_CNT), where STMT_LIST is used indirectly
    if(p->data.type != STMT_LIST) {
        this = create_node(STMT_LIST);
        append_node(this,p);
    }else {
        this = p;
    }

    switch (t->type){
        // STMT_LIST    -> e
        case TOKEN_R_CURLY:
            return true;
            // STMT_LIST    -> STMT ; STMT_LIST
        default:
            return(s_stmt(t, this) &&
            s_stmt_list(t, this));
    }
}

bool s_stmt(token* const t, node* p){
    STATE_PRINT
    switch(t->type){
            // STMT  -> ASSIGN_OR_CALL_OR_LABEL_STMT
        case TOKEN_IDENT:
        case TOKEN_UNDESCORE:
            return s_acl_stmt(t, p);
            // STMT ->  VAR_DEF ;
        case TOKEN_KW_VAR:
        case TOKEN_KW_CONST:
            return s_var_def_stmt(t, p)
                && apt_t(t, TOKEN_SEMICOLON);
            // STMT ->  IF_STMT
        case TOKEN_KW_IF:
            return s_if_stmt(t, p);
            // STMT ->  WHILE_STMT
        case TOKEN_KW_WHILE:
            return s_while_stmt(t, p);
            // STMT -> RET_STMT ;
        case TOKEN_KW_RETURN:
            return s_ret_stmt(t, p)
                   && apt_t(t, TOKEN_SEMICOLON);
            // STMT -> FOR_STMT
        case TOKEN_KW_FOR:
            return s_for_stmt(t,p);
            // STMT -> CONTINUE_STMT ;
        case TOKEN_KW_CONTINUE:
            return s_continue_stmt(t,p)&&
                   apt_t(t, TOKEN_SEMICOLON);
            // STMT -> BREAK_STMT ;
        case TOKEN_KW_BREAK:
            return s_break_stmt(t,p)&&
                   apt_t(t, TOKEN_SEMICOLON);
            // ERR
        default:
            return false;
    }
}

bool s_var_def_stmt(token* const t, node* p){
    STATE_PRINT
    node_type nd = ((t->type == TOKEN_KW_VAR)?VAR_DEF:CONST_DEF);
    node* this = create_node(nd);
    append_node(this,p);
    // VAR_CON id TYPE_AND_ASSIGN
    return (s_var_con(t,this)
        && apt_t_cpy_id(t,this, false)
        && s_type_and_assign(t,this));
}

bool s_var_con(token* const t, node* p){
    STATE_PRINT
    switch (t->type) {
        case TOKEN_KW_CONST:
            p->data.type = CONST_DEF;
            apt_t(t,TOKEN_KW_CONST);
            return true;
        case TOKEN_KW_VAR:
            p->data.type = VAR_DEF;
            apt_t(t,TOKEN_KW_VAR);
            return true;
        default:
            return false;
    }
}

bool s_type_and_assign(token* const t, node* p){
    STATE_PRINT
    return (s_opt_type(t,p) &&
    apt_t(t,TOKEN_ASSIGN) &&
    s_expr(t,p));
}

bool s_opt_type(token* const t, node* p){
    STATE_PRINT
    switch(t->type){
            // OPT_TYPE ->  : D_TYPE
        case TOKEN_COLON:
            return apt_t(t,TOKEN_COLON)
            && s_definition_type(t,p);
            // OPT_TYPE -> e
        default:
            p->data.ret_value = UNKNOWN;
            return true;
    }
}

bool s_definition_type(token* const t, node* p){
    STATE_PRINT
    switch(t->type){
        //D_TYPE		->	[]u8
        case TOKEN_KW_SLICE:
            p->data.ret_value = U8_SLICE;
            break;
            //D_TYPE		->	?[]u8
        case TOKEN_KW_SLICE_NULL:
            p->data.ret_value = U8_SLICE_OPTIONAL;
            break;
            //D_TYPE		->	i32
        case TOKEN_KW_INT:
            p->data.ret_value = I32;
            break;
            //D_TYPE		->	?i32
        case TOKEN_KW_INT_NULL:
            p->data.ret_value = I32_OPTIONAL;
            break;
            //D_TYPE		->	f64
        case TOKEN_KW_FLOAT:
            p->data.ret_value = F64;
            break;
            //D_TYPE		->	?f64
        case TOKEN_KW_FLOAT_NULL:
            p->data.ret_value = F64_OPTIONAL;
            break;
        default:
            return false;
    }
    g_err = get_token(t);
    return !g_err;
}


bool s_acl_stmt(token* const t, node* p){
    STATE_PRINT
    node* this = create_node(VAR); // placeholder node.
    append_node(this,p);
    // SPECIAL CASE :  '_' character can be accepted, but it must be an Assign statement.
    if(t->type == TOKEN_UNDESCORE){
        this->data.type = ASSIGN_THROW_AWAY;
        apt_t(t,TOKEN_UNDESCORE);
        return !g_err
        && s_assign_stmt(t,this)
        && apt_t(t,TOKEN_SEMICOLON);
    }

    apt_t_cpy_id(t,this, true);
    switch (t->type) {
        // ACL  ->  FUNC_CALL_STMT ;
        case TOKEN_L_PAREN:
            return s_func_call_stmt(t,this)
            && apt_t(t, TOKEN_SEMICOLON);
        // ACL  ->  ASSIGN_STMT ;
        case TOKEN_ASSIGN:
            return s_assign_stmt(t,this)
            && apt_t(t, TOKEN_SEMICOLON);
        // ACL  ->  WHILE_NAME WHILE_STMT
        case TOKEN_COLON:
            return apt_t(t,TOKEN_COLON)
                && s_while_stmt(t,this);
        default:
            return false;
    }
}

bool s_assign_stmt(token* const t, node* p){
    STATE_PRINT
    if(p->data.type != ASSIGN_THROW_AWAY)
        p->data.type = ASSIGN;
    return apt_t(t,TOKEN_ASSIGN) && s_expr(t,p);
}

bool s_if_stmt(token* const t, node* p){
    STATE_PRINT
    node* this = create_node(IF);
    append_node(this,p);
    return apt_t(t,TOKEN_KW_IF)
    && s_cond(t,this)
    && s_if_block(t,this)
    && s_else_block(t,this);
}

bool s_cond(token* const t, node* p){
    STATE_PRINT
    node* this = create_node(COND);
    append_node(this,p);
    return apt_t(t,TOKEN_L_PAREN)
    && s_expr(t,this)
    && apt_t(t,TOKEN_R_PAREN)
    && s_null_cond(t,this);
}

bool s_null_cond(token* const t, node* p){
    STATE_PRINT
    node* this;
    switch(t->type){
        case TOKEN_VERT_BAR:
            this = create_node(NULL_COND);
            append_node(this,p);
            return apt_t(t,TOKEN_VERT_BAR)
            && apt_t_cpy_id(t,this, false)
            && apt_t(t,TOKEN_VERT_BAR);
        default:
            return true;
    }
}

bool s_if_block(token* const t, node* p){
    // Note: STMT_LISTS are created regardless of a single STMT or STMT_LIST.
    //       s_stmt_list has a prevention of having a duplicate STMT_LIST
    STATE_PRINT
    node *this = create_node(STMT_LIST);
    append_node(this, p);
    return apt_t(t,TOKEN_L_CURLY)
        && s_stmt_list(t,this)
        && apt_t(t,TOKEN_R_CURLY);
}

bool s_else_block(token* const t, node* p){
    STATE_PRINT
    node* this;
    switch(t->type){
        // ELSE_BLOCK   -> else ELSE_PART
        case TOKEN_KW_ELSE:
            this = create_node(ELSE);
            append_node(this,p);
            return apt_t(t,TOKEN_KW_ELSE)
            && s_else_part(t,this);
        // ELSE_BLOCK   -> e
        default:
            return true;
    }
}

bool s_else_part(token* const t, node* p){
    STATE_PRINT
    node *this = create_node(STMT_LIST);
    append_node(this, p);

    switch(t->type){
        case TOKEN_KW_IF:
            return s_stmt(t,this);
        case TOKEN_L_CURLY:
            return apt_t(t,TOKEN_L_CURLY)
            && s_stmt_list(t,this)
            && apt_t(t,TOKEN_R_CURLY);

        default:
            return false;
    }
}


bool s_func_call_stmt(token* const t, node* p){
    STATE_PRINT
    p->data.type = FUNC;
    return apt_t(t,TOKEN_L_PAREN)
    && s_input_param_list(t,p)
    && apt_t(t,TOKEN_R_PAREN);
}

bool s_input_param_list(token* const t, node* p){
    STATE_PRINT
    node* this;
    // Check if passed token is INPUT_PARAM_LIST
    if(p->data.type != INPUT_PARAM_LIST) {
        this = create_node(INPUT_PARAM_LIST);
        if(!this) return false;
        append_node(this, p);
    }else{
        this = p;
    }

    switch(t->type){
        // INPUT_PARAM_LIST ->  e
        // case TOKEN_KW_RETURN:
        case TOKEN_R_PAREN:
            return true;
        // INPUT_PARAM_LIST ->  expr IP_SEP
        default:
            return s_input_param(t,this);
    }
}

bool s_input_param(token* const t, node *p){
    STATE_PRINT
    node* this = create_node(INPUT_PARAM);
    append_node(this,p);
    // connect EXPR to INPUT_PARAM, go to IP_SEP and pass INPUT_PARAM_LIST
    return s_expr(t,this) && s_ip_sep(t,p);
}

bool s_ip_sep(token* const t, node *p){
    STATE_PRINT
    switch(t->type){
        // IP_SEP   ->  e
        case TOKEN_R_PAREN:
            return true;
        case TOKEN_COMMA:
            return apt_t(t,TOKEN_COMMA) && s_input_param_list(t,p);
        default:
            return false;
    }
}

bool s_opt_expr(token* const t, node* p){
    STATE_PRINT
    node* this = create_node(OPT_EXPR);
    append_node(this,p);
    switch(t->type){
        // OPT_EXPR -> e
        case TOKEN_SEMICOLON:
            this->data.ret_value = DT_NULL;
            return true;
        // OPT_EXPR -> expr
        default:
            this->data.ret_value = UNKNOWN;
            return s_expr(t,this);
    }
}

bool s_ret_stmt(token* const t, node* p){
    STATE_PRINT
    node* this = create_node(RETURN);
    append_node(this,p);
    return apt_t(t,TOKEN_KW_RETURN) && s_opt_expr(t,this);
}

// Note: ID je implicitne typu i32.
bool s_for_null(token* const t, node* p){
    STATE_PRINT
    node* this = create_node(NULL_COND);
    append_node(this,p);
    return apt_t(t,TOKEN_VERT_BAR)
    && apt_t_cpy_id(t,this, false)
    && apt_t(t,TOKEN_VERT_BAR);
}


bool s_for_stmt(token* const t, node* p){
    STATE_PRINT
    node* this = create_node(FOR);
    append_node(this,p);
    node* cond_node = create_node(COND);
    append_node(cond_node,this);
            return apt_t(t,TOKEN_KW_FOR)
                && apt_t(t,TOKEN_L_PAREN)
                && s_expr(t,cond_node)
                && apt_t(t,TOKEN_R_PAREN)
                && s_for_null(t,cond_node) &&
                s_for_block(t,this);

}

bool s_for_block(token* const t, node* p){
    STATE_PRINT
    // FOR_BLOCK -> { STMT_LIST }
    return apt_t(t,TOKEN_L_CURLY)
        && s_stmt_list(t,p)
        && apt_t(t,TOKEN_R_CURLY);
}


bool s_while_stmt(token* const t, node* p){
    STATE_PRINT
    node* this;
    if(p->data.type == VAR){ // WHILE_NAME has not been empty (prev state = ACL_STMT)
        this = p;
        p->data.type = WHILE; // WHILE stores LABEL value
    }else{
        this = create_node(WHILE);
        append_node(this,p);
    }
    return apt_t(t,TOKEN_KW_WHILE)
            && s_cond(t,this)
            && s_spec_while_cnt(t,this)
            && s_spec_while_if(t,this)
            && s_spec_while_else(t,this);
}

bool s_spec_while_cnt(token* const t, node* p){
    STATE_PRINT
    if(t->type != TOKEN_COLON) return true;

    apt_t(t,TOKEN_COLON);
    switch (t->type) {
        // SPEC_WHILE_CNT   ->  ( CNT_BLOCK )
        case TOKEN_L_PAREN:
            return apt_t(t,TOKEN_L_PAREN)
                && s_cnt_block(t,p)
                && apt_t(t,TOKEN_R_PAREN);
        // SPEC_WHILE_CNT   ->  e
        default:
            return true;
    }
}

bool s_cnt_block(token* const t, node* p){
    STATE_PRINT
    node *this = create_node(WHILE_CNT);
    append_node(this,p);
    node* stmt_list;
    switch(t->type){
        // CNT_BLOCK -> : { STMT_LIST }
        case TOKEN_COLON:
            return apt_t(t, TOKEN_COLON)
            && apt_t(t,TOKEN_L_CURLY)
            && s_stmt_list(t,this)
            && apt_t(t,TOKEN_R_CURLY);
        // CNT_BLOCK -> STMT
        default:
            stmt_list = create_node(STMT_LIST);
            append_node(stmt_list,this);
            return s_stmt(t,stmt_list);
    }
}

bool s_spec_while_if(token* const t, node* p){ // basically a while block
    STATE_PRINT
    // SPEC_WHILE_IF -> { STMT_LIST }
        return apt_t(t,TOKEN_L_CURLY)
        && s_stmt_list(t,p)
        && apt_t(t,TOKEN_R_CURLY);
}

bool s_spec_while_else(token* const t, node* p){
    STATE_PRINT
    node *this;
    switch(t->type){
        // S_SPEC_WHILE_ELSE    ->  S_SPEC_WHILE_BLOCK
        case TOKEN_KW_ELSE:
            this = create_node(WHILE_ELSE);
            append_node(this,p);
            return apt_t(t,TOKEN_KW_ELSE)
                && s_spec_while_else_block(t,this);
        // S_SPEC_WHILE_ELSE    ->  e
        default:
            return true;
    }
}

bool s_spec_while_else_block(token* const t, node* p){
    STATE_PRINT
    // SPEC_WHILE_ELSE -> { STMT_LIST }
        return apt_t(t,TOKEN_L_CURLY)
        && s_stmt_list(t,p)
        && apt_t(t,TOKEN_R_CURLY);
        // SPEC_WHILE_ELSE -> STMT
}

bool s_opt_label(token* const t, node* p){
    STATE_PRINT
    switch (t->type) {
        // OPT_LABEL    ->  id
        case TOKEN_IDENT:
            return apt_t_cpy_id(t,p, false);
        // OPT_LABEL    ->  e
        default:
            return true;
    }
}

bool s_continue_stmt(token* const t, node* p){
    STATE_PRINT
    node* this = create_node(CONTINUE);
    append_node(this,p);
    return apt_t(t,TOKEN_KW_CONTINUE)
    && s_opt_label(t,this);
}

bool s_break_stmt(token* const t, node* p){
    STATE_PRINT
    node* this = create_node(BREAK);
    append_node(this,p);
    return apt_t(t,TOKEN_KW_BREAK)
    && s_opt_label(t,this);
}

#ifndef NDEBUG
#define SUCCESS_PRINT \
        if(success)              \
            fprintf(stderr, "\x1b[2m" "Parsing exit with success" "\x1b[0m" "\n"); \
        else \
            fprintf(stderr, "\x1b[31m" "Parsing exit without success" "\n" \
            "last known Token Type: %d" "\x1b[0m" "\n",t->type);

#else
#define SUCCESS_PRINT
#endif


error_code parse(node *syn_root){
    token *t = malloc(sizeof (token));
    if (t==NULL) return INTERNAL_ERR;

    if((g_err = get_token(t))) return g_err;
    bool success = s_start(t, syn_root);
    SUCCESS_PRINT

    if(!success){
        if(g_err) return g_err;
        return SYNTACTIC_ERR;
    }
    free(t);
    return 0;
}




// Expression are parsed topdown with recursive descent.
#define CATCH_ERR_T \
    *err = get_token(t); \
    if(*err) return n;

#define CATCH_ERR_NODE(node_type) \
    node* this = create_node(node_type); \
    if(this == NULL){             \
        *err = INTERNAL_ERR;               \
        return n;\
    }

#define CREATE_NODE(name,node_type) \
    node* name = create_node(node_type);\
    if(name == NULL){\
        *err = INTERNAL_ERR;\
        return NULL;\
    }

node_type relational_op_nt(const token* t){
    switch (t->type) {
        case TOKEN_EQ: return EQUAL;
        case TOKEN_NOT_EQ: return NOT_EQUAL;
        case TOKEN_LESS_EQ: return EQUALLESS;
        case TOKEN_GREATER_EQ: return EQUALMORE;
        case TOKEN_GREATER_THAN: return MORE;
        case TOKEN_LESS_THAN: return LESS;
        default: return 0;
    }
}

node_type plus_minus_nt(const token* t){
    if(t->type == TOKEN_PLUS) return ADD;
    if(t->type == TOKEN_MINUS) return SUB;
    return 0;
}

node_type mul_div_nt(const token* t){
    if(t->type == TOKEN_MUL) return MUL;
    if(t->type == TOKEN_DIV) return DIV;
    return 0;
}

node_type neg_nt(const token* t){
    if(t->type == TOKEN_MINUS) return INT_NEGATE;
    if(t->type == TOKEN_NEG) return B_NEG;
    return 0;
}



node* s_primary(token* const t, error_code* err, bool or_else){
    CREATE_NODE(prim,ERR)
    long len;
    switch (t->type) {
        case TOKEN_KW_TRUE:
            prim->data.type = LITERAL;
            prim->data.ret_value = BOOLEAN;
            prim->data.bool_value = true;
            *err = get_token(t);
            break;
        case TOKEN_KW_FALSE:
            prim->data.type = LITERAL;
            prim->data.ret_value = BOOLEAN;
            prim->data.bool_value = false;
            *err = get_token(t);
            break;
        case TOKEN_LIT_INT:
            prim->data.type = LITERAL;
            prim->data.ret_value = I32;
            prim->data.int_value = t->content.int_lit;
            *err = get_token(t);
            break;
        case TOKEN_LIT_FLOAT:
            prim->data.type = LITERAL;
            prim->data.ret_value = F64;
            prim->data.float_value = t->content.float_lit;
            *err = get_token(t);
            break;
        case TOKEN_KW_NULL:
            prim->data.type = NULL_LIT;
            prim->data.ret_value = DT_NULL;
            *err = get_token(t);
            break;
        case TOKEN_LIT_STRING:
            prim->data.type = LITERAL;
            prim->data.ret_value = STRING_LITERAL;
            len = strlen(t->content.string_lit);
            prim->data.str_value = (char*)malloc((sizeof(char)*len)+1);
            if(prim->data.str_value == NULL){
                delete_tree(&prim);
                *err = INTERNAL_ERR;
                return NULL;
            }
            strcpy(prim->data.str_value,t->content.string_lit);
            *err = get_token(t);
            break;
        case TOKEN_AS:
            prim->data.type = AS_FUNC;
            prim->data.ret_value = I32;
            bool res = apt_t(t,TOKEN_AS)
                    && apt_t(t,TOKEN_L_PAREN)
                    && apt_t(t,TOKEN_KW_INT)
                    && apt_t(t,TOKEN_COMMA)
                    && apt_t_cpy_id(t,prim,false)
                    && apt_t(t,TOKEN_R_PAREN);
            if(!res) *err = g_err;
            break;
        case TOKEN_IDENT:
            prim->data.type = VAR;
            prim->data.ret_value = UNKNOWN;
            apt_t_cpy_id(t,prim, true);
            *err = g_err;
            if(*err) break;
            if(t->type == TOKEN_L_PAREN){
                s_func_call_stmt(t,prim);
            }else{
                if(has_namespace(prim)){
                    *err = SYNTACTIC_ERR;
                }
            }
            break;
        case TOKEN_L_PAREN:
            prim->data.type = EXPR;
            *err = get_token(t);
            if(*err) break;
            s_expr(t,prim);
            if(t->type != TOKEN_R_PAREN){
                *err = SYNTACTIC_ERR;
            }
            *err = get_token(t);
            break;
        case TOKEN_KW_IF: // TERNARY
            prim->data.type = IF_TERNARY;
            *err = get_token(t);
            if(*err) break;
            // if (cond) expr else expr
            if(!(s_cond(t,prim)
                && s_expr(t,prim)
                && apt_t(t,TOKEN_KW_ELSE)
                && s_expr(t,prim))){
                if(g_err) *err = g_err;
                else *err = SYNTACTIC_ERR;
            }
            break;
        case TOKEN_KW_UNREACHABLE:
            if(!or_else){
                *err = SYNTACTIC_ERR;
                break;
            }
            prim->data.type = UNREACHABLE;
            prim->data.ret_value = VOID;
            *err = get_token(t);
            break;

        default:
            ERROR_PRINT("%s: INVALID PRIMARY OPERAND, got %d", __func__,t->type)
            delete_tree(&prim);
            *err = SYNTACTIC_ERR;
            return NULL;
    }

    if(*err){
        if(prim->data.ret_value == U8 || prim->data.ret_value == UNKNOWN){
            free(prim->data.id_name);
            prim->data.id_name = NULL;
        }
        delete_tree(&prim);
        return NULL;
    }
    return prim;
}



node* s_postfix(token* const t,error_code* err, bool or_else){
    node* n = s_primary(t,err, or_else);
    if(t->type == TOKEN_IS_UNREACHABLE){
        CREATE_NODE(this,IS_UNREACHABLE)
        *err = get_token(t);
        if(*err) return NULL;
        append_node(n,this);
        n = this;
    }
    return n;
}

node* s_negate(token* const t,error_code* err, bool or_else){
    if(neg_nt(t)){
        node_type nt = neg_nt(t);
        CREATE_NODE(this,nt)
        if (*err) return NULL;
        *err = get_token(t);
        if (*err) return NULL;
        append_node(s_negate(t,err, false),this);
        return this;
    }
    return s_postfix(t,err, or_else);
}

node* s_times_divide(token* const t,error_code* err, bool or_else){
    node* n = s_negate(t,err, or_else);
    while(mul_div_nt(t)){
        node_type nt = mul_div_nt(t);
        CATCH_ERR_T
        CATCH_ERR_NODE(nt)
        append_node(n,this);
        append_node(s_negate(t,err,false),this);
        n = this;
    }
    return n;
}

node* s_plus_minus(token* const t, error_code* err, bool or_else){
    node* n = s_times_divide(t,err, or_else);
    while(plus_minus_nt(t)){
        node_type nt = plus_minus_nt(t);
        CATCH_ERR_T
        CATCH_ERR_NODE(nt)
        append_node(n,this);
        append_node(s_times_divide(t,err, false),this);
        n = this;
    }
    return n;
}

node* s_or_else(token* const t, error_code* err){
    node* n = s_plus_minus(t,err,0);
    while(t->type == TOKEN_ORELSE){
        CATCH_ERR_T
        CATCH_ERR_NODE(ORELSE)
        append_node(n,this);
        append_node(s_plus_minus(t,err,1),this);
        n = this;
    }
    return n;
}

node* s_equality(token* const t, error_code* err){
    node* n = s_or_else(t,err);
    while(relational_op_nt(t)){
        node_type nt = relational_op_nt(t);
        CATCH_ERR_T
        CATCH_ERR_NODE(nt)
        append_node(n,this);
        append_node(s_or_else(t,err),this);
        n = this;
    }
    return n;
}

node* s_logical_and(token* const t,error_code* err){
    node* n = s_equality(t,err);
    while(t->type == TOKEN_AND){
        CATCH_ERR_T
        CATCH_ERR_NODE(B_AND)
        append_node(n,this);
        append_node(s_equality(t,err),this);
        n = this;
    }
    return n;
}

node* s_logical_or(token* const t,error_code* err){
    node* n = s_logical_and(t,err);
    while(t->type == TOKEN_OR){
        CATCH_ERR_T
        CATCH_ERR_NODE(B_OR)
        append_node(n,this);
        append_node(s_logical_and(t,err),this);
        n = this;
    }
    return n;
}


#ifndef NDEBUG
    #define S_EXPR_EXIT \
        fprintf(stderr,"\x1b[2m" "Expression parsing exited with code %d" "\x1b[0m" "\n",err);
#else
    #define S_EXPR_EXIT
#endif

bool s_expr(token* const t, node* p){
    STATE_PRINT
    error_code err = NO_ERR;
    node* parsed_expr = s_logical_or(t,&err);
    node* this = create_node(EXPR);
    append_node(parsed_expr,this);
    append_node(this,p);
    g_err = err;
    S_EXPR_EXIT
    return !err;
}

