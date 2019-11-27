#ifndef LEXER_H
#define LEXER_H

#include <stdio.h>
#include <stdarg.h>

#define MAX_TOKEN_LEN 257
#define PARSING (FILE*)1

//tokens pg 20
typedef enum Token
{
	END,

	EQUALEQUAL, GREATER, GTEQ, LESS, LTEQ, NOTEQUAL,
	LOGICAL_OR, LOGICAL_AND, LOGICAL_NEGATION, INCREMENT, DECREMENT,
	LEFT_SHIFT, RIGHT_SHIFT, BIT_AND, BIT_OR, BIT_XOR,
	BIT_NEGATION,

	DOT, INDIRECTION,

	/* types and type qualifiers */
	CHAR, SHORT, INT, LONG, FLOAT, DOUBLE, VOID, SIGNED, UNSIGNED,

	/* These are not yet used */
	CONST, BOOL, COMPLEX, IMAGINARY, AUTO, VOLATILE, REGISTER, RESTRICT,
	INLINE, EXTERN, ENUM, STATIC, STRUCT, UNION, TYPEDEF,

	/* other keywords (PRINT is my own till I can make a printf-like function) */
	DO, WHILE, FOR, PRINT, IF, ELSE, SWITCH, CASE, DEFAULT, CONTINUE,
	BREAK, GOTO, RETURN, SIZEOF,

	ID,

	MOD, LPAREN, RPAREN, MULT, ADD, SUB, DIV, COLON, SEMICOLON,
	EQUAL, COMMA, LBRACKET, RBRACKET, LBRACE, RBRACE, TERNARY,

	/* compound assignment operators */
	ADDEQUAL, SUBEQUAL, MULTEQUAL, DIVEQUAL, MODEQUAL,
	LSHIFT_EQUAL, RSHIFT_EQUAL, BIT_AND_EQUAL, BIT_OR_EQUAL, BIT_XOR_EQUAL,

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
//		char* str_literal; just use id
		int int_val;
		float float_val;
		double double_val;
	} v;
} token_value;

typedef struct token_lex
{
	token_value tok;
	unsigned int line;
	unsigned int pos; //pos on line
	char* char_pos; //location in overall file string (only used in read_token_from_str)
} token_lex;

void free_token_lex(void* tok_lex);


typedef struct lexer_state
{
	char* cur_file;
	unsigned int cur_line;
	unsigned int cur_pos;
	unsigned int cur_tok;
	unsigned int cur_char;
} lexer_state;


void lex_error(lexer_state* lex, char *str, ...);

token_lex read_token(FILE* file, lexer_state* lex_state, FILE* preprocessed);
token_lex read_token_from_str(char* input, lexer_state* lex_state, FILE* preprocessed);

void print_token(token_value* tok, FILE* file, int print_enum);
int print_token_to_str(token_value* tok, char* buf, size_t size);


#endif
