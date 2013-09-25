#ifndef LEXER_H
#define LEXER_H

#include <stdio.h>

#define MAX_TOKEN_LEN 256

//tokens pg 20
typedef enum {
	ERROR, END,
	
	EQUALEQUAL, GREATER, GTEQ, LESS, LTEQ, NOTEQUAL,
	LOGICAL_OR, LOGICAL_AND, LOGICAL_NEGATION, INCREMENT, DECREMENT,

	/* types and type qualifiers */
	INT, SHORT, LONG, FLOAT, DOUBLE, CHAR, VOID, SIGNED, UNSIGNED,
	
	/* other keywords */
	DO, WHILE, FOR, PRINT, IF, ELSE, SWITCH, CASE, DEFAULT, CONTINUE, BREAK, GOTO, RETURN,
	
	ID,

	MOD='%', LPAREN='(', RPAREN=')', MULT='*', ADD='+', SUB='-', DIV='/', COLON=':', SEMICOLON=';',
	EQUAL='=', COMMA=',', LBRACKET='[', RBRACKET=']', LBRACE='{', RBRACE='}', TERNARY,

	/* compound assignment operators */
	ADDEQUAL, SUBEQUAL, MULTEQUAL, DIVEQUAL, MODEQUAL,

	/* literals aka constants */
	INT_LITERAL, FLOAT_LITERAL, DOUBLE_LITERAL, CHAR_LITERAL, STR_LITERAL,


	/* for internal use/hacks, not really tokens */
	LABEL, DECLARATION, EXP, EXPR_LIST, FUNC_CALL,
	POST_INCREMENT, POST_DECREMENT, PRE_INCREMENT, PRE_DECREMENT
} Token;

typedef struct token_value
{
	Token type;
	union {
		char* id;
		int int_val;
		float float_val;
		double double_val;
	} v;
} token_value;


token_value read_token(FILE* file);

void print_token(token_value* tok, FILE* file, int print_enum);


#endif
