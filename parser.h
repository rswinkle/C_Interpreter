#ifndef PARSER_H
#define PARSER_H



#include <stdlib.h>
#include <stdarg.h>
#include <ctype.h>

#include "cvector.h"

#define MAX_TOKEN_LEN 256

#define GET_STMT(VEC, I) GET_VOID(VEC, statement, I)

typedef enum {
	GREATER, GTEQ, LESS, LTEQ, NOTEQUAL, VAR, WHILE, PRINT, IF, NUM, ID, END, ERROR,
	PLUS='+', MINUS='-', DIV='/', MULT='*', EQUAL='=', COLON=':', SEMICOLON=';',
	COMMA=',', LBRACE='{', RBRACE='}'
} Token;




typedef struct operand
{
	Token type;
	char* id;
	int num;
} operand;


typedef enum {
	NULL_STMT = 0, WHILE_STMT, PRINT_STMT, ASSIGN_STMT, IF_STMT, GOTO_STMT
} stmt_type;


typedef struct statement
{
	stmt_type type;
	int jump_to;
	char* lvalue;
	operand left;
	operand right;
	Token op;
} statement;

#define INIT_STATEMENT(stmt) \
	stmt.type =

//void init_statement(statement* stmt, stmt_type 

void free_statement(void* stmt);

void parse_error(const char* str);

void parse_program();
void var_decl();
void body();
void id_list();
void statement_list();
void assign();
void expr(statement* statement);
void while_stmt();
void if_stmt();
void print_stmt();
void p_condition(statement* statement);



#endif
