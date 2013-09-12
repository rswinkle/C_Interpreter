#ifndef PARSER_H
#define PARSER_H



#include <stdlib.h>
#include <stdarg.h>
#include <ctype.h>


#include "cvector.h"

#define MAX_TOKEN_LEN 256

#define GET_STMT(VEC, I) GET_VOID(VEC, statement, I)
#define GET_TOKEN_VAL(VEC, I) GET_VOID((VEC), token_value, (I))


//tokens pg 20
typedef enum {
	ERROR, END,
	
	EQUALEQUAL, GREATER, GTEQ, LESS, LTEQ, NOTEQUAL,
	LOGICAL_OR, LOGICAL_AND,

	/* types and type qualifiers */
	INT, SHORT, LONG, FLOAT, DOUBLE, CHAR, VOID, SIGNED, UNSIGNED,
	
	/* other keywords */
	DO, WHILE, PRINT, IF, ELSE, SWITCH, CASE, DEFAULT, CONTINUE, BREAK, GOTO,
	
	ID,

	MOD='%', LPAREN='(', RPAREN=')', MULT='*', ADD='+', SUB='-', DIV='/', COLON=':', SEMICOLON=';',
	EQUAL='=', COMMA=',', LBRACKET='[', RBRACKET=']', LBRACE='{', RBRACE='}',

	/* compound assignment operators */
	ADDEQUAL, SUBEQUAL, MULTEQUAL, DIVEQUAL, MODEQUAL,

	/* literals aka constants */
	INT_LITERAL, FLOAT_LITERAL, CHAR_LITERAL, STR_LITERAL,


	/* for internal use/hacks, not really tokens */
	LABEL, EXP
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
	DOUBLE_TYPE
} var_type;



typedef struct var_value
{
	var_type type;
	union {
		int int_val;
		double double_val;
	} v;
} var_value;
	
#define GET_VAR_VALUE(VEC, I) GET_VOID(VEC, var_value, I)


void free_var_value(void* var);



typedef struct
{
	Token type;
	union {
		char* id;
		int integer;
		double real;
	} v;
} token_value;

typedef struct
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


typedef enum {
	NULL_STMT = 0, WHILE_STMT, PRINT_STMT, EXPR_STMT, IF_STMT, GOTO_STMT,

} stmt_type;


typedef struct
{
	stmt_type type;
	size_t jump_to;
	char* lvalue;
	expression* exp;
	var_type vtype;
} statement;

#define INIT_STATEMENT(stmt) \
	stmt.type =

//void init_statement(statement* stmt, stmt_type 
//


/*********************************/
typedef struct function
{
	vector_void stmt_list;	
	
	vector_str variables;
	vector_void values;
} function;


typedef struct
{
	
	vector_void stmt_list;
	size_t pc;
	vector_str global_variables;
	vector_void global_values;
} program_state;

/***************************/




void print_token(token_value* tok);

expression* copy_expr(expression*);
void free_statement(void* stmt);

void parse_error(const char* str);

void parse_program(program_state* prog, FILE* file);
void declaration_list(parsing_state* p, program_state* prog);
void declaration(parsing_state* p, program_state* prog);
var_type declaration_specifier(parsing_state* p, program_state* prog);
void initialized_declarator_list(parsing_state* p, program_state* prog, var_type v_type);
void initialized_declarator(parsing_state* p, program_state* prog, var_type v_type);





void body(parsing_state* p, program_state* prog);
void statement_list(parsing_state* p, program_state* prog);

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
void primary_expr(parsing_state* p, program_state* prog, expression* exp);


void while_stmt(parsing_state* p, program_state* prog);
void if_stmt(parsing_state* p, program_state* prog);
void print_stmt(parsing_state* p, program_state* prog);
void goto_stmt(parsing_state* p, program_state* prog);



#endif
