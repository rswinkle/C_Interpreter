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

	POUND, //only used in preprocessor

	/* for internal use/hacks, not really tokens */
	DECLARATION, EXP, EXPR_LIST, FUNC_CALL,
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

typedef struct token_lex
{
	token_value tok;
	unsigned int line;
	unsigned int pos;
} token_lex;



typedef struct lexer_state
{
	unsigned int cur_line;
	unsigned int cur_pos;
	unsigned int cur_tok;
} lexer_state;

//create struct lexer
//holds current line and position
//add line and maybe position to token_value ?



token_lex read_token(FILE* file, lexer_state* lex_state);

void print_token(token_value* tok, FILE* file, int print_enum);
int print_token_to_str(token_value* tok, char* buf, size_t size);


#endif
