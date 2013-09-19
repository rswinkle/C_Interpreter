#ifndef PARSER_H
#define PARSER_H



#include <stdlib.h>
#include <stdarg.h>
#include <ctype.h>

#include "clist.h"

#include "cvector.h"

#define MAX_TOKEN_LEN 256

#define GET_STMT(VEC, I) GET_VOID(VEC, statement, I)
#define GET_TOKEN_VAL(VEC, I) GET_VOID((VEC), token_value, (I))
#define GET_FUNCTION(VEC, I) GET_VOID(VEC, function, I)
#define GET_SYMBOL(VEC, I) GET_VOID(VEC, symbol, I)
#define GET_BINDING(VEC, I) GET_VOID(VEC, binding, I)


//tokens pg 20
typedef enum {
	ERROR, END,
	
	EQUALEQUAL, GREATER, GTEQ, LESS, LTEQ, NOTEQUAL,
	LOGICAL_OR, LOGICAL_AND, LOGICAL_NEGATION,

	/* types and type qualifiers */
	INT, SHORT, LONG, FLOAT, DOUBLE, CHAR, VOID, SIGNED, UNSIGNED,
	
	/* other keywords */
	DO, WHILE, FOR, PRINT, IF, ELSE, SWITCH, CASE, DEFAULT, CONTINUE, BREAK, GOTO, RETURN,
	
	ID,

	MOD='%', LPAREN='(', RPAREN=')', MULT='*', ADD='+', SUB='-', DIV='/', COLON=':', SEMICOLON=';',
	EQUAL='=', COMMA=',', LBRACKET='[', RBRACKET=']', LBRACE='{', RBRACE='}',

	/* compound assignment operators */
	ADDEQUAL, SUBEQUAL, MULTEQUAL, DIVEQUAL, MODEQUAL,

	/* literals aka constants */
	INT_LITERAL, FLOAT_LITERAL, CHAR_LITERAL, STR_LITERAL,


	/* for internal use/hacks, not really tokens */
	LABEL, DECLARATION, EXP, EXPR_LIST, FUNC_CALL
} Token;



typedef enum
{
	UNKNOWN = 0,

	CHAR_TYPE, /* implementation defined whether plain char is signed or not ...*/
	UCHAR_TYPE,

	SHORT_TYPE,
	USHORT_TYPE,

	INT_TYPE,
	SINT_TYPE,

	FLOAT_TYPE,
	DOUBLE_TYPE,

	FUNCTION_TYPE,
	VOID_TYPE
} var_type;



typedef struct var_value
{
	var_type type;
	union {
		int int_val;
		double double_val;
		unsigned int func_loc; //change to direct pointer?
	} v;
//	list_head list;
} var_value;
	
#define GET_VAR_VALUE(VEC, I) GET_VOID(VEC, var_value, I)


void free_var_value(void* var);
void free_active_binding_list(void* l);



typedef struct token_value
{
	Token type;
	union {
		char* id;
		int integer;
		double real;
	} v;
} token_value;

typedef struct parsing_state
{
	vector_void tokens;
	unsigned int pos;
	unsigned i;
} parsing_state;


typedef struct expression expression;

struct expression
{
	token_value tok;
	expression* left;
	expression* right;
};

typedef struct expr_block
{
	expression* data;
	size_t n;
	size_t used;
} expr_block;


typedef enum {
	NULL_STMT = 0, WHILE_STMT, PRINT_STMT, EXPR_STMT, IF_STMT, GOTO_STMT,
	CONTINUE_STMT, BREAK_STMT,
	DECL_STMT, RETURN_STMT, START_COMPOUND_STMT, END_COMPOUND_STMT
} stmt_type;


typedef struct binding
{
	char* name;
	var_type vtype;
	int decl_stmt;
} binding;

typedef struct active_binding
{
	var_value val;
	int parent;
	list_head list;
} active_binding;


typedef struct statement
{
	stmt_type type;
	size_t jump_to;
	char* lvalue;
	expression* exp;
	var_type vtype;
	
	long parent;

	vector_void* bindings;
} statement;

#define INIT_STATEMENT(stmt) \
	stmt.type =

//void init_statement(statement
//* stmt, stmt_type 
//



/*********************************/

typedef struct symbol
{
	char* name;
	//vector_void of var_values
	// or
	
	int cur_parent;
	list_head head;
} symbol;

typedef struct function
{
	vector_void stmt_list;
	size_t pc;
	var_value ret_val;
	int n_params;
	vector_void symbols;

	vector_str labels;
	vector_i label_locs;
} function;




typedef struct program_state
{
	vector_void functions;
	vector_void* stmt_list;
	size_t* pc;
	function* func;


	int cur_parent; //<-- location of open block statement
	int cur_iter;
	int cur_iter_switch;

	vector_str global_variables;
	vector_void global_values;

	vector_void expressions;
	vector_str string_db;
	vector_void* bindings;
} program_state;

/***************************/



int make_expression_block(size_t n, expr_block* block);

void free_statement(void* stmt);

//should probably make these static in parser.c
void print_token(token_value* tok);
void print_statement(statement* stmt);
expression* copy_expr(program_state* prog, expression* e);
expression* make_expression(program_state* prog);

void parse_error(token_value* tok, char *str, ...);
token_value* peek_token(parsing_state* p, long offset);
void parse_seek(parsing_state* p, int origin, long offset);


void parse_program(program_state* prog, FILE* file);
void translation_unit(parsing_state* p, program_state* prog);
void top_level_declaration(parsing_state* p, program_state* prog);

void function_definition(parsing_state* p, program_state* prog);
void function_declarator(parsing_state* p, program_state* prog, var_type vtype);
void parameter_list(parsing_state* p, program_state* prog);
void parameter_declaration(parsing_state* p, program_state* prog);


//void declaration_list(parsing_state* p, program_state* prog);
void declaration(parsing_state* p, program_state* prog);
var_type declaration_specifier(parsing_state* p, program_state* prog, int match);
void initialized_declarator_list(parsing_state* p, program_state* prog, var_type v_type);
void initialized_declarator(parsing_state* p, program_state* prog, var_type v_type);





void compound_statement(parsing_state* p, program_state* prog);
void declaration_or_statement_list(parsing_state* p, program_state* prog);
void declaration_or_statement(parsing_state* p, program_state* prog);
void statement_rule(parsing_state* p, program_state* prog);

void expression_stmt(parsing_state* p, program_state* prog);
void expr(parsing_state* p, program_state* prog, expression* e);
void comma_expr(parsing_state* p, program_state* prog, expression* e);
void assign_expr(parsing_state* p, program_state* prog, expression* e);


void cond_expr(parsing_state* p, program_state* prog, expression* e);
void logical_or_expr(parsing_state* p, program_state* prog, expression* e);
void logical_and_expr(parsing_state* p, program_state* prog, expression* e);
void equality_expr(parsing_state* p, program_state* prog, expression* e);
void relational_expr(parsing_state* p, program_state* prog, expression* e);

void add_expr(parsing_state* p, program_state* prog, expression* exp);
void mult_expr(parsing_state* p, program_state* prog, expression* exp);

void unary_expr(parsing_state* p, program_state* prog, expression* e);
void postfix_expr(parsing_state* p, program_state* prog, expression* e);
void logical_negation_expr(parsing_state* p, program_state* prog, expression* e);

void function_call(parsing_state* p, program_state* prog, expression* e);
void expression_list(parsing_state* p, program_state* prog, expression* e);

void primary_expr(parsing_state* p, program_state* prog, expression* exp);


void while_stmt(parsing_state* p, program_state* prog);
void break_or_continue_stmt(parsing_state* p, program_state* prog);
void if_stmt(parsing_state* p, program_state* prog);
void print_stmt(parsing_state* p, program_state* prog);
void goto_stmt(parsing_state* p, program_state* prog);
void return_stmt(parsing_state* p, program_state* prog);
void labeled_stmt(parsing_state* p, program_state* prog);


//can I put these prototypes here too?
int execute_expr(program_state* prog, expression* e);

void add_binding(program_state* prog, char* name, var_type vtype);
void remove_binding(program_state* prog, char* name);
void clear_bindings(program_state* prog);
void pop_scope(program_state* prog);
int is_ancestor(program_state* prog, int parent, int child);
void apply_scope(program_state* prog, int jump_to, int child, int parent);
void remove_scope(program_state* prog, int jump_to, int child, int parent);
int find_lowest_common_ancestor(program_state* prog, int parent1, int parent2);


enum
{
	ONLY_LOCAL,
	ONLY_GLOBAL,
	BOTH
};



symbol* look_up_symbol(program_state* prog, const char* var);
var_value* look_up_value(program_state* prog, const char* var, int search);
unsigned int look_up_loc(program_state* prog, const char* var);

#endif
