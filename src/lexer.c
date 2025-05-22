
#include "lexer.h"

#include "c_utils.h"
//#include "cvector.h"

#include <ctype.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#define HANDLE_BACKSLASH() \
do { \
	c = getc(file); \
	while (c == '\\') { \
		c = getc(file); \
		if (c != '\n') \
			goto stray_backslash; \
		c = getc(file); \
	} \
	lex_state->cur_pos++; \
} while (0)


// TODO handle
// \nnn octal
// \xhh hex
// \uhhhh non-ASCII unicode below 10000 hex (C99)
// \Uhhhhhhhh non-ASCII unicode h is hex digit
// TODO combine with HANDLE_BACKSLASH, use bool flag arg
// and escaped_quote should also be a parameter
// Also need to fix line numbers and cur_pos
// then maybe with HANDLE_BACKSLASH_STR()
#define HANDLE_BACKSLASH_ESCS() \
do { \
	escaped_quote = 0; \
	c = getc(file); \
	while (c == '\\') { \
		c = getc(file); \
		if (c != '\n') { \
			switch (c) { \
			case 'a': c = 0x07; break; \
			case 'b': c = 0x08; break; \
			case 'e': c = 0x1B; break; \
			case 'f': c = 0x0C; break; \
			case 'n': c = 0x0A; break; \
			case 'r': c = 0x0D; break; \
			case 't': c = 0x09; break; \
			case 'v': c = 0x0B; break; \
			case '\"': escaped_quote = 1; break;\
			case '\\': \
			case '\'': \
			case '\?': \
				break; \
			default:   \
				goto unrecognized_esc_seq; \
			} \
			break; \
		} else { \
			c = getc(file); \
		} \
	} \
	lex_state->cur_pos++; \
} while (0)


//preprocessed is either NULL or a valid output file for preprocessing output
//or it's the macro PARSING (1 currently but could be any invalid non-NULL pointer) which means we're parsing
token_lex read_token(FILE* file, lexer_state* lex_state, FILE* preprocessed)
{
	static char token_buf[MAX_TOKEN_LEN+1];
	int c, i = 0, tmp;
	int escaped_quote = 0;

	token_lex tok_lex = { 0 };

start:

	while (1) {
		HANDLE_BACKSLASH();

		if (c == '\n') {
			lex_state->cur_pos = 1;
			lex_state->cur_tok = 0;
			lex_state->cur_line++;
		}

		if (!isspace(c))
			break;

		if (preprocessed && preprocessed != PARSING) //could do 1 bit operation
			putc(c, preprocessed);
	}

	lex_state->cur_tok++;

	tok_lex.pos = lex_state->cur_pos - 1;
	tok_lex.line = lex_state->cur_line;

	//printf("getting token starting with '%c'\n", c);
	switch (c) {
	case ':': tok_lex.tok.type = COLON;         break;
	case ',': tok_lex.tok.type = COMMA;         break;
	case ';': tok_lex.tok.type = SEMICOLON;     break;
	case '{': tok_lex.tok.type = LBRACE;        break;
	case '}': tok_lex.tok.type = RBRACE;        break;
	case '(': tok_lex.tok.type = LPAREN;        break;
	case ')': tok_lex.tok.type = RPAREN;        break;
	case '?': tok_lex.tok.type = TERNARY;       break;
	case '.': tok_lex.tok.type = DOT;           break;
	case '~': tok_lex.tok.type = BIT_NEGATION;  break;

	//not really a valid token but used in preprocessor and for having correct
	//file and line numbers in error messages
	case '#':
		tok_lex.tok.type = POUND;
		if (preprocessed == PARSING) { //only used during parsing
			tok_lex = read_token(file, lex_state, NULL);
			if (tok_lex.tok.type != INT_LITERAL)
				lex_error(lex_state, "invalid token\n");

			lex_state->cur_line = tok_lex.tok.v.int_val;

			tok_lex = read_token(file, lex_state, NULL);
			if (tok_lex.tok.type != STR_LITERAL)
				lex_error(lex_state, "invalid token\n");

			free(lex_state->cur_file);
			lex_state->cur_file = mystrdup(tok_lex.tok.v.id);
			free(tok_lex.tok.v.id);
			goto start;
		}
		break;

	case '+':
		HANDLE_BACKSLASH();
		if (c == '+') {
			tok_lex.tok.type = INCREMENT;
		} else if (c == '=') {
			tok_lex.tok.type = ADDEQUAL;
		} else {
			ungetc(c, file);
			lex_state->cur_pos--;
			tok_lex.tok.type = ADD;
		}
		break;

	case '-':
		HANDLE_BACKSLASH();
		if (c == '-') {
			tok_lex.tok.type = DECREMENT;
		} else if (c == '=') {
			tok_lex.tok.type = SUBEQUAL;
		} else {
			ungetc(c, file);
			lex_state->cur_pos--;
			tok_lex.tok.type = SUB;
		}
		break;

	case '*':
		HANDLE_BACKSLASH();
		if (c == '=') {
			tok_lex.tok.type = MULTEQUAL;
		} else {
			ungetc(c, file);
			lex_state->cur_pos--;
			tok_lex.tok.type = MULT;
		}
		break;

	case '/':
		HANDLE_BACKSLASH();
		if (c == '=') {
			tok_lex.tok.type = DIVEQUAL;
		} else if (c == '/') { // it's a single line comment
			while (1) {
				HANDLE_BACKSLASH();
				if (c == '\n') {
					ungetc(c, file);
					lex_state->cur_pos--;
					break;
				}
			}
			goto start;
		} else if (c == '*') { /* start of block comment */
			while (1) {
				c = getc(file);
				lex_state->cur_pos++;
				if (c == '*') {
					HANDLE_BACKSLASH();
					if (c == '/')
						goto start;
				}
			}
		} else {
			ungetc(c, file);
			lex_state->cur_pos--;
			tok_lex.tok.type = DIV;
		}
		break;

	case '%':
		HANDLE_BACKSLASH();
		if (c == '=') {
			tok_lex.tok.type = MODEQUAL;
		} else {
			ungetc(c, file);
			lex_state->cur_pos--;
			tok_lex.tok.type = MOD;
		}
		break;

	case '=':
		HANDLE_BACKSLASH();
		if (c == '=') {
			tok_lex.tok.type = EQUALEQUAL;
		} else {
			ungetc(c, file);
			lex_state->cur_pos--;
			tok_lex.tok.type = EQUAL;
		}
		break;

	case '|':
		HANDLE_BACKSLASH();
		if (c == '|') {
			tok_lex.tok.type = LOGICAL_OR;
		} else if (c == '=') {
			tok_lex.tok.type = BIT_OR_EQUAL;
		} else {
			ungetc(c, file);
			lex_state->cur_pos--;
			tok_lex.tok.type = BIT_OR;
		}
		break;

	case '&':
		HANDLE_BACKSLASH();
		if (c == '&') {
			tok_lex.tok.type = LOGICAL_AND;
		} else if (c == '=') {
			tok_lex.tok.type = BIT_AND_EQUAL;
		} else {
			ungetc(c, file);
			lex_state->cur_pos--;
			tok_lex.tok.type = BIT_AND;
		}
		break;

	case '^':
		HANDLE_BACKSLASH();
		if (c == '=') {
			tok_lex.tok.type = BIT_XOR_EQUAL;
		} else {
			ungetc(c, file);
			lex_state->cur_pos--;
			tok_lex.tok.type = BIT_XOR;
		}
		break;

	case '!':
		HANDLE_BACKSLASH();
		if (c == '=') {
			tok_lex.tok.type = NOTEQUAL;
		} else {
			ungetc(c, file);
			lex_state->cur_pos--;
			tok_lex.tok.type = LOGICAL_NEGATION;
		}
		break;


	case '<':
		HANDLE_BACKSLASH();
		if (c == '=') {
			tok_lex.tok.type = LTEQ;
		} else if (c == '<') {
			HANDLE_BACKSLASH();
			if (c == '=') {
				tok_lex.tok.type = LSHIFT_EQUAL;
			} else {
				ungetc(c, file);
				lex_state->cur_pos--;
				tok_lex.tok.type = LEFT_SHIFT;
			}
		} else {
			ungetc(c, file);
			lex_state->cur_pos--;
			tok_lex.tok.type = LESS;
		}
		break;

	case '>':
		HANDLE_BACKSLASH();
		if (c == '=') {
			tok_lex.tok.type = GTEQ;
		} else if (c == '>') {
			HANDLE_BACKSLASH();
			if (c == '=') {
				tok_lex.tok.type = RSHIFT_EQUAL;
			} else {
				ungetc(c, file);
				lex_state->cur_pos--;
				tok_lex.tok.type = RIGHT_SHIFT;
			}
		} else {
			ungetc(c, file);
			lex_state->cur_pos--;
			tok_lex.tok.type = GREATER;
		}
		break;

	case '\'':
		HANDLE_BACKSLASH_ESCS();
		tmp = c; //save character
		HANDLE_BACKSLASH();
		if (c != '\'') {
			lex_error(lex_state, "multi-character character constant\n");
		}
		tok_lex.tok.type = CHAR_LITERAL;
		tok_lex.tok.v.int_val = tmp;
		break;

	default:

		//TODO should +532 and -12321 be handled
		//as literals or unary +/- and 2 positive literals?
		//and what about hex and oct literals?  crap
		if (isdigit(c)) {
			while (isdigit(c)) {
				token_buf[i++] = c;
				HANDLE_BACKSLASH();

				if (i > MAX_TOKEN_LEN)
					goto token_length_error;
			}

			if (c == '.') {
				if (i > MAX_TOKEN_LEN)
					goto token_length_error;

				token_buf[i++] = c;
				HANDLE_BACKSLASH();

				while (isdigit(c)) {
					token_buf[i++] = c;
					HANDLE_BACKSLASH();

					if (i > MAX_TOKEN_LEN)
						goto token_length_error;
				}

				ungetc(c, file);
				lex_state->cur_pos--;
				token_buf[i] = '\0';
				tok_lex.tok.type = DOUBLE_LITERAL;
				tok_lex.tok.v.double_val = atof(token_buf);
			} else {
				ungetc(c, file);
				lex_state->cur_pos--;
				token_buf[i] = '\0';

				tok_lex.tok.type = INT_LITERAL;
				tok_lex.tok.v.int_val = atoi(token_buf);
			}

		} else if (isalpha(c)) {
			while (isalnum(c) || c == '_') {
				token_buf[i++] = c;
				HANDLE_BACKSLASH();

				if (i > MAX_TOKEN_LEN)
					goto token_length_error;
			}
			ungetc(c, file);
			lex_state->cur_pos--;
			token_buf[i] = '\0';

			// TODO
			//should turn these off for preprocessor ... though I think you deserve problems
			//if you define macros with the same names as keywords
			//could also optimize this, check length, turn into a mini state machine etc.
			if (!strcmp(token_buf, "do")) {         tok_lex.tok.type = DO;         break; }
			if (!strcmp(token_buf, "while")) {      tok_lex.tok.type = WHILE;      break; }
			if (!strcmp(token_buf, "for")) {        tok_lex.tok.type = FOR;        break; }
			if (!strcmp(token_buf, "if")) {         tok_lex.tok.type = IF;         break; }
			if (!strcmp(token_buf, "else")) {       tok_lex.tok.type = ELSE;       break; }
			if (!strcmp(token_buf, "print")) {      tok_lex.tok.type = PRINT;      break; }
			if (!strcmp(token_buf, "goto")) {       tok_lex.tok.type = GOTO;       break; }
			if (!strcmp(token_buf, "return")) {     tok_lex.tok.type = RETURN;     break; }
			if (!strcmp(token_buf, "switch")) {     tok_lex.tok.type = SWITCH;     break; }
			if (!strcmp(token_buf, "break")) {      tok_lex.tok.type = BREAK;      break; }
			if (!strcmp(token_buf, "sizeof")) {     tok_lex.tok.type = SIZEOF;     break; }

			if (!strcmp(token_buf, "case")) {       tok_lex.tok.type = CASE;       break; }
			if (!strcmp(token_buf, "default")) {    tok_lex.tok.type = DEFAULT;    break; }
			if (!strcmp(token_buf, "continue")) {   tok_lex.tok.type = CONTINUE;   break; }

			if (!strcmp(token_buf, "char")) {       tok_lex.tok.type = CHAR;       break; }
			if (!strcmp(token_buf, "short")) {      tok_lex.tok.type = SHORT;      break; }
			if (!strcmp(token_buf, "int")) {        tok_lex.tok.type = INT;        break; }
			if (!strcmp(token_buf, "long")) {       tok_lex.tok.type = LONG;       break; }
			if (!strcmp(token_buf, "signed")) {     tok_lex.tok.type = SIGNED;     break; }
			if (!strcmp(token_buf, "unsigned")) {   tok_lex.tok.type = UNSIGNED;   break; }
			if (!strcmp(token_buf, "double")) {     tok_lex.tok.type = DOUBLE;     break; }
			if (!strcmp(token_buf, "float")) {      tok_lex.tok.type = FLOAT;      break; }
			if (!strcmp(token_buf, "void")) {       tok_lex.tok.type = VOID;       break; }

			tok_lex.tok.type = ID;
			tok_lex.tok.v.id = mystrdup(token_buf);
			assert(tok_lex.tok.v.id);

		} else if (c == '"') {
			HANDLE_BACKSLASH_ESCS();
			while (c != '"' || escaped_quote) {
				token_buf[i++] = c;
				HANDLE_BACKSLASH_ESCS();
				if (i > MAX_TOKEN_LEN)
					goto token_length_error;
			}
			token_buf[i] = '\0';

			tok_lex.tok.type = STR_LITERAL;
			tok_lex.tok.v.id = mystrdup(token_buf);
			assert(tok_lex.tok.v.id);

		} else if (c == EOF) {
			tok_lex.tok.type = END;
			tok_lex.pos = 1;
			tok_lex.line = lex_state->cur_line + 1;
		} else {
			lex_error(lex_state, "Unrecognized token \"%c\"", c);
		}
	}

	if (preprocessed && preprocessed != PARSING && tok_lex.tok.type != POUND &&
	    tok_lex.tok.type != ID && tok_lex.tok.type != END) {
		print_token(&tok_lex.tok, preprocessed, 0);
	}

	//print_token(&tok_lex.tok, stdout, 0);
	//printf(" ");
	//print_token(&tok_lex.tok, stdout, 1);
	//puts("");
	//fflush(stdout);

	return tok_lex;

unrecognized_esc_seq:
	lex_error(lex_state, "Unknown escape sequence in program\n");

stray_backslash:
	lex_error(lex_state, "stray \\ in program (perhaps you have a space between it and a newline)\n");

token_length_error:
	lex_error(lex_state, "Token length is too long, max token length is %d\n", MAX_TOKEN_LEN);
	
	// never gets here, gets rid of compiler warning
	return tok_lex;
}

void lex_error(lexer_state* lex, char *str, ...)
{
	va_list args;
	va_start(args, str);

	fprintf(stderr, "%s:%u:%u: Lexer Error: ", lex->cur_file, lex->cur_line, lex->cur_pos);
	vfprintf(stderr, str, args);

	va_end(args);

	exit(0);
}


#define HANDLE_BACKSLASH_STR() \
do { \
	++c; \
	while (*c == '\\') { \
		++c; \
		if (*c != '\n') \
			goto stray_backslash; \
		++c; \
	} \
	lex_state->cur_pos++; \
} while (0)

#define HANDLE_BACKSLASH_STR_ESCS() \
do { \
	escaped_quote = 0; \
	++c; \
	while (*c == '\\') { \
		++c; \
		if (*c != '\n') { \
			switch (*c) { \
			case 'a': *c = 0x07; break; \
			case 'b': *c = 0x08; break; \
			case 'e': *c = 0x1B; break; \
			case 'f': *c = 0x0C; break; \
			case 'n': *c = 0x0A; break; \
			case 'r': *c = 0x0D; break; \
			case 't': *c = 0x09; break; \
			case 'v': *c = 0x0B; break; \
			case '\"': escaped_quote = 1; break;\
			case '\\': \
			case '\'': \
			case '\?': \
				break; \
			default:   \
				goto unrecognized_esc_seq; \
			} \
			break; \
		} else { \
			++c; \
		} \
	} \
	lex_state->cur_pos++; \
} while (0)


token_lex read_token_from_str(char* input, lexer_state* lex_state, FILE* preprocessed)
{
	static char token_buf[MAX_TOKEN_LEN+1];
	int i = 0, tmp;
	int escaped_quote;

	token_lex tok_lex = { 0 };

	char* c = &input[lex_state->cur_char - 1]; //start 1 before for increment

start:

	while (1) {
		HANDLE_BACKSLASH_STR();

		if (*c == '\n') {
			lex_state->cur_pos = 1;
			lex_state->cur_tok = 0;
			lex_state->cur_line++;
		}

		if (!isspace(*c))
			break;

		if (preprocessed && preprocessed != PARSING) //could do 1 bit operation
			putc(*c, preprocessed);
	}

	lex_state->cur_tok++;

	tok_lex.pos = lex_state->cur_pos - 1;
	tok_lex.line = lex_state->cur_line;
	tok_lex.char_pos = c;

//	printf("getting token starting with '%c'\n", c);
	switch (*c) {
	case ':': tok_lex.tok.type = COLON;         break;
	case ',': tok_lex.tok.type = COMMA;         break;
	case ';': tok_lex.tok.type = SEMICOLON;     break;
	case '{': tok_lex.tok.type = LBRACE;        break;
	case '}': tok_lex.tok.type = RBRACE;        break;
	case '(': tok_lex.tok.type = LPAREN;        break;
	case ')': tok_lex.tok.type = RPAREN;        break;
	case '?': tok_lex.tok.type = TERNARY;       break;
	case '.': tok_lex.tok.type = DOT;           break;
	case '~': tok_lex.tok.type = BIT_NEGATION;  break;

	//not really a valid token but used in preprocessor and for having correct
	//file and line numbers in error messages
	case '#':
		tok_lex.tok.type = POUND;
		if (preprocessed == PARSING) {
			lex_state->cur_char += c - input + 1;

			tok_lex = read_token_from_str(input, lex_state, NULL);
			if (tok_lex.tok.type != INT_LITERAL)
				lex_error(lex_state, "invalid token");

			lex_state->cur_line = tok_lex.tok.v.int_val;

			tok_lex = read_token_from_str(input, lex_state, NULL);
			if (tok_lex.tok.type != STR_LITERAL)
				lex_error(lex_state, "invalid token");

			free(lex_state->cur_file);
			lex_state->cur_file = mystrdup(tok_lex.tok.v.id); //think about when to free this;
			free(tok_lex.tok.v.id);
			goto start;
		}
		break;

	case '+':
		HANDLE_BACKSLASH_STR();
		if (*c == '+') {
			tok_lex.tok.type = INCREMENT;
		} else if (*c == '=') {
			tok_lex.tok.type = ADDEQUAL;
		} else {
			--c;
			lex_state->cur_pos--;
			tok_lex.tok.type = ADD;
		}
		break;

	case '-':
		HANDLE_BACKSLASH_STR();
		if (*c == '-') {
			tok_lex.tok.type = DECREMENT;
		} else if (*c == '=') {
			tok_lex.tok.type = SUBEQUAL;
		} else {
			--c;
			lex_state->cur_pos--;
			tok_lex.tok.type = SUB;
		}
		break;

	case '*':
		HANDLE_BACKSLASH_STR();
		if (*c == '=') {
			tok_lex.tok.type = MULTEQUAL;
		} else {
			--c;
			lex_state->cur_pos--;
			tok_lex.tok.type = MULT;
		}
		break;

	case '/':
		HANDLE_BACKSLASH_STR();
		if (*c == '=') {
			tok_lex.tok.type = DIVEQUAL;
		} else if (*c == '/') { // it's a single line comment
			while (1) {         // yes you can have escaped multi line single line comments
				HANDLE_BACKSLASH_STR();
				if (*c == '\n') {
					--c;
					lex_state->cur_pos--;
					break;
				}
			}
			goto start;
		} else if (*c == '*') { /* start of block comment */
			while (1) {
				++c;
				lex_state->cur_pos++;
				if (*c == '*') {
					HANDLE_BACKSLASH_STR();
					if (*c == '/')
						goto start;
				}
			}
		} else {
			--c;
			lex_state->cur_pos--;
			tok_lex.tok.type = DIV;
		}
		break;

	case '%':
		HANDLE_BACKSLASH_STR();
		if (*c == '=') {
			tok_lex.tok.type = MODEQUAL;
		} else {
			--c;
			lex_state->cur_pos--;
			tok_lex.tok.type = MOD;
		}
		break;

	case '=':
		HANDLE_BACKSLASH_STR();
		if (*c == '=') {
			tok_lex.tok.type = EQUALEQUAL;
		} else {
			--c;
			lex_state->cur_pos--;
			tok_lex.tok.type = EQUAL;
		}
		break;

	case '|':
		HANDLE_BACKSLASH_STR();
		if (*c == '|') {
			tok_lex.tok.type = LOGICAL_OR;
		} else if (*c == '=') {
			tok_lex.tok.type = BIT_OR_EQUAL;
		} else {
			--c;
			lex_state->cur_pos--;
			tok_lex.tok.type = BIT_OR;
		}
		break;

	case '&':
		HANDLE_BACKSLASH_STR();
		if (*c == '&') {
			tok_lex.tok.type = LOGICAL_AND;
		} else if (*c == '=') {
			tok_lex.tok.type = BIT_AND_EQUAL;
		} else {
			--c;
			lex_state->cur_pos--;
			tok_lex.tok.type = BIT_AND;
		}
		break;

	case '^':
		HANDLE_BACKSLASH_STR();
		if (*c == '=') {
			tok_lex.tok.type = BIT_XOR_EQUAL;
		} else {
			--c;
			lex_state->cur_pos--;
			tok_lex.tok.type = BIT_XOR;
		}
		break;

	case '!':
		HANDLE_BACKSLASH_STR();
		if (*c == '=') {
			tok_lex.tok.type = NOTEQUAL;
		} else {
			--c;
			lex_state->cur_pos--;
			tok_lex.tok.type = LOGICAL_NEGATION;
		}
		break;


	case '<':
		HANDLE_BACKSLASH_STR();
		if (*c == '=') {
			tok_lex.tok.type = LTEQ;
		} else if (*c == '<') {
			HANDLE_BACKSLASH_STR();
			if (*c == '=') {
				tok_lex.tok.type = LSHIFT_EQUAL;
			} else {
				--c;
				lex_state->cur_pos--;
				tok_lex.tok.type = LEFT_SHIFT;
			}
		} else {
			--c;
			lex_state->cur_pos--;
			tok_lex.tok.type = LESS;
		}
		break;

	case '>':
		HANDLE_BACKSLASH_STR();
		if (*c == '=') {
			tok_lex.tok.type = GTEQ;
		} else if (*c == '>') {
			HANDLE_BACKSLASH_STR();
			if (*c == '=') {
				tok_lex.tok.type = RSHIFT_EQUAL;
			} else {
				--c;
				lex_state->cur_pos--;
				tok_lex.tok.type = RIGHT_SHIFT;
			}
		} else {
			--c;
			lex_state->cur_pos--;
			tok_lex.tok.type = GREATER;
		}
		break;

	case '\'':
		HANDLE_BACKSLASH_STR_ESCS();
		tmp = *c; //save character
		HANDLE_BACKSLASH_STR();
		if (*c != '\'') {
			lex_error(lex_state, "multi-character character constant\n");
		}
		tok_lex.tok.type = CHAR_LITERAL;
		tok_lex.tok.v.int_val = tmp;
		break;

	default:
		if (isdigit(*c)) {
			while (isdigit(*c)) {
				token_buf[i++] = *c;
				HANDLE_BACKSLASH_STR();

				if (i > MAX_TOKEN_LEN)
					goto token_length_error;
			}

			if (*c == '.') {
				if (i > MAX_TOKEN_LEN)
					goto token_length_error;

				token_buf[i++] = *c;
				HANDLE_BACKSLASH_STR();

				while (isdigit(*c)) {
					token_buf[i++] = *c;
					HANDLE_BACKSLASH_STR();

					if (i > MAX_TOKEN_LEN)
						goto token_length_error;
				}

				--c;
				lex_state->cur_pos--;
				token_buf[i] = '\0';
				tok_lex.tok.type = DOUBLE_LITERAL;
				tok_lex.tok.v.double_val = atof(token_buf);
			} else {
				--c;
				lex_state->cur_pos--;
				token_buf[i] = '\0';

				tok_lex.tok.type = INT_LITERAL;
				tok_lex.tok.v.int_val = atoi(token_buf);
			}

		} else if (isalpha(*c)) {
			while (isalnum(*c) || *c == '_') {
				token_buf[i++] = *c;
				HANDLE_BACKSLASH_STR();

				if (i > MAX_TOKEN_LEN)
					goto token_length_error;
			}
			--c;
			lex_state->cur_pos--;
			token_buf[i] = '\0';

			//should turn these off for preprocessor ... though I think you deserve problems
			//if you define macros with the same names as keywords
			if (!strcmp(token_buf, "do")) {         tok_lex.tok.type = DO;         break; }
			if (!strcmp(token_buf, "while")) {      tok_lex.tok.type = WHILE;      break; }
			if (!strcmp(token_buf, "for")) {        tok_lex.tok.type = FOR;      break; }
			if (!strcmp(token_buf, "if")) {         tok_lex.tok.type = IF;         break; }
			if (!strcmp(token_buf, "else")) {       tok_lex.tok.type = ELSE;       break; }
			if (!strcmp(token_buf, "print")) {      tok_lex.tok.type = PRINT;      break; }
			if (!strcmp(token_buf, "goto")) {       tok_lex.tok.type = GOTO;       break; }
			if (!strcmp(token_buf, "return")) {     tok_lex.tok.type = RETURN;     break; }
			if (!strcmp(token_buf, "switch")) {     tok_lex.tok.type = SWITCH;     break; }
			if (!strcmp(token_buf, "break")) {      tok_lex.tok.type = BREAK;      break; }

			if (!strcmp(token_buf, "case")) {       tok_lex.tok.type = CASE;       break; }
			if (!strcmp(token_buf, "default")) {    tok_lex.tok.type = DEFAULT;    break; }
			if (!strcmp(token_buf, "continue")) {   tok_lex.tok.type = CONTINUE;   break; }

			if (!strcmp(token_buf, "char")) {       tok_lex.tok.type = CHAR;      break; }
			if (!strcmp(token_buf, "short")) {      tok_lex.tok.type = SHORT;      break; }
			if (!strcmp(token_buf, "int")) {        tok_lex.tok.type = INT;        break; }
			if (!strcmp(token_buf, "long")) {       tok_lex.tok.type = LONG;        break; }
			if (!strcmp(token_buf, "signed")) {     tok_lex.tok.type = SIGNED;     break; }
			if (!strcmp(token_buf, "unsigned")) {   tok_lex.tok.type = UNSIGNED;   break; }
			if (!strcmp(token_buf, "double")) {     tok_lex.tok.type = DOUBLE;     break; }
			if (!strcmp(token_buf, "float")) {      tok_lex.tok.type = FLOAT;      break; }
			if (!strcmp(token_buf, "void")) {       tok_lex.tok.type = VOID;       break; }

			tok_lex.tok.type = ID;
			tok_lex.tok.v.id = mystrdup(token_buf);
			assert(tok_lex.tok.v.id);

		} else if (*c == '"') {
			HANDLE_BACKSLASH_STR_ESCS();
			while (*c != '"' || escaped_quote) {
				token_buf[i++] = *c;
				HANDLE_BACKSLASH_STR_ESCS();
				if (i > MAX_TOKEN_LEN)
					goto token_length_error;
			}
			token_buf[i] = '\0';

			tok_lex.tok.type = STR_LITERAL;
			tok_lex.tok.v.id = mystrdup(token_buf);
			assert(tok_lex.tok.v.id);

		} else if (*c == '\0') {
			tok_lex.tok.type = END;
			tok_lex.pos = 1;
			tok_lex.line = lex_state->cur_line + 1;
		} else {
			lex_error(lex_state, "Unrecognized token \"%c\"", c);
		}
	}

	if (preprocessed && preprocessed != PARSING && tok_lex.tok.type != POUND &&
	    tok_lex.tok.type != ID && tok_lex.tok.type != END)
		print_token(&tok_lex.tok, preprocessed, 0);

//	print_token(&tok_lex.tok, stdout, 0);
//	fflush(stdout);

	//so we know the index to start with for next call
	lex_state->cur_char += c - input + 1;

	return tok_lex;

unrecognized_esc_seq:
	lex_error(lex_state, "Unknown escape sequence in program\n");

stray_backslash:
	lex_error(lex_state, "stray \\ in program (perhaps you have a space between it and a newline)\n");

token_length_error:
	lex_error(lex_state, "Token length is too long, max token length is %d\n", MAX_TOKEN_LEN);

	// never gets here, gets rid of compiler warning
	return tok_lex;
}











void free_token_lex(void* tok_lex)
{
	token_lex* t = tok_lex;
	if (t->tok.type == ID || t->tok.type == STR_LITERAL)
		free(t->tok.v.id);
}


char* insert_esc_seqs(char* s, char* out)
{
	assert(out);
	char* o = out;
	for (; *s; s++, o++) {
		switch (*s) {
		case 0x07: *o++ = '\\'; *o = 'a'; break;
		case 0x08: *o++ = '\\'; *o = 'b'; break;
		case 0x1B: *o++ = '\\'; *o = 'e'; break;
		case 0x0C: *o++ = '\\'; *o = 'f'; break;
		case 0x0A: *o++ = '\\'; *o = 'n'; break;
		case 0x0D: *o++ = '\\'; *o = 'r'; break;
		case 0x09: *o++ = '\\'; *o = 't'; break;
		case 0x0B: *o++ = '\\'; *o = 'v'; break;
		case '\"': *o++ = '\\'; *o = '\"'; break;
		case '\\': *o++ = '\\'; *o = '\\'; break;
		case '\'': *o++ = '\\'; *o = '\''; break;

		// don't really need this since no one uses trigraphs
		//case '\?': *o++ = '\\'; *o = '\?'; break;
		default:
			*o = *s;
		}
	}
	return out;
}

char* insert_char_esc_seq(int val, char* out)
{
	char* o = out;
	o[0] = val;
	o[1] = 0;
	o[2] = 0;
	switch (val) {
	case 0x07: o[0] = '\\'; o[1] = 'a'; break;
	case 0x08: o[0] = '\\'; o[1] = 'b'; break;
	case 0x1B: o[0] = '\\'; o[1] = 'e'; break;
	case 0x0C: o[0] = '\\'; o[1] = 'f'; break;
	case 0x0A: o[0] = '\\'; o[1] = 'n'; break;
	case 0x0D: o[0] = '\\'; o[1] = 'r'; break;
	case 0x09: o[0] = '\\'; o[1] = 't'; break;
	case 0x0B: o[0] = '\\'; o[1] = 'v'; break;
	case '\"': o[0] = '\\'; o[1] = '\"'; break;
	case '\\': o[0] = '\\'; o[1] = '\\'; break;
	case '\'': o[0] = '\\'; o[1] = '\''; break;
	}
	return out;
}




void print_token(token_value* tok, FILE* file, int print_enum)
{
	// max needed is if every character was an escape sequence
	char buf[2*MAX_TOKEN_LEN+1];

	if (print_enum) {
		switch (tok->type) {
			case END:              fprintf(file, "END");     break;
			case EQUALEQUAL:       fprintf(file, "EQUALEQUAL");     break;
			case GREATER:          fprintf(file, "GREATER");     break;
			case GTEQ:             fprintf(file, "GTEQ");     break;
			case LESS:             fprintf(file, "LESS");     break;
			case LTEQ:             fprintf(file, "LTEQ");     break;
			case DOT:              fprintf(file, "DOT");     break;
			case NOTEQUAL:         fprintf(file, "NOTEQUAL");     break;
			case LOGICAL_OR:       fprintf(file, "LOGICAL_OR");     break;
			case LOGICAL_AND:      fprintf(file, "LOGICAL_AND");     break;
			case LOGICAL_NEGATION: fprintf(file, "LOGICAL_NEGATION");     break;
			case INT:              fprintf(file, "INT");     break;
			case SHORT:            fprintf(file, "SHORT");     break;
			case LONG:             fprintf(file, "LONG");     break;
			case FLOAT:            fprintf(file, "FLOAT");     break;
			case DOUBLE:           fprintf(file, "DOUBLE");     break;
			case CHAR:             fprintf(file, "CHAR");     break;
			case VOID:             fprintf(file, "VOID");     break;
			case SIGNED:           fprintf(file, "SIGNED");     break;
			case UNSIGNED:         fprintf(file, "UNSIGNED");     break;
			case DO:               fprintf(file, "DO");     break;
			case FOR:              fprintf(file, "FOR");    break;
			case WHILE:            fprintf(file, "WHILE");     break;
			case PRINT:            fprintf(file, "PRINT");     break;
			case IF:               fprintf(file, "IF");     break;
			case ELSE:             fprintf(file, "ELSE");     break;
			case SWITCH:           fprintf(file, "SWITCH");     break;
			case CASE:             fprintf(file, "CASE");     break;
			case DEFAULT:          fprintf(file, "DEFAULT");     break;
			case CONTINUE:         fprintf(file, "CONTINUE");     break;
			case RETURN:           fprintf(file, "RETURN");         break;
			case SIZEOF:           fprintf(file, "SIZEOF");         break;
			case BREAK:            fprintf(file, "BREAK");     break;
			case GOTO:             fprintf(file, "GOTO");     break;
			case ID:               fprintf(file, "ID = %s\n", tok->v.id);     break;
			case MOD:              fprintf(file, "MOD");     break;
			case LPAREN:           fprintf(file, "LPAREN");     break;
			case RPAREN:           fprintf(file, "RPAREN");     break;
			case MULT:             fprintf(file, "MULT");     break;
			case ADD:              fprintf(file, "ADD");     break;
			case SUB:              fprintf(file, "SUB");     break;
			case DIV:              fprintf(file, "DIV");     break;
			case COLON:            fprintf(file, "COLON");     break;
			case SEMICOLON:        fprintf(file, "SEMICOLON");     break;
			case EQUAL:            fprintf(file, "EQUAL");     break;
			case COMMA:            fprintf(file, "COMMA");     break;
			case LBRACKET:         fprintf(file, "LBRACKET");     break;
			case RBRACKET:         fprintf(file, "RBRACKET");     break;
			case LBRACE:           fprintf(file, "LBRACE");     break;
			case RBRACE:           fprintf(file, "RBRACE");     break;
			case INCREMENT:        fprintf(file, "INCREMENT");     break;
			case DECREMENT:        fprintf(file, "DECREMENT");     break;

			case LEFT_SHIFT:       fprintf(file, "LEFT_SHIFT");    break;
			case RIGHT_SHIFT:      fprintf(file, "RIGHT_SHIFT");    break;
			case BIT_AND:          fprintf(file, "BIT_AND");     break;
			case BIT_OR:           fprintf(file, "BIT_OR");     break;
			case BIT_XOR:          fprintf(file, "BIT_XOR");     break;
			case BIT_NEGATION:     fprintf(file, "BIT_NEGATION");     break;

			case TERNARY:          fprintf(file, "TERNARY");     break;

			case ADDEQUAL:         fprintf(file, "ADDEQUAL");     break;
			case SUBEQUAL:         fprintf(file, "SUBEQUAL");     break;
			case MULTEQUAL:        fprintf(file, "MULTEQUAL");     break;
			case DIVEQUAL:         fprintf(file, "DIVEQUAL");     break;
			case MODEQUAL:         fprintf(file, "MODEQUAL");     break;
			case LSHIFT_EQUAL:     fprintf(file, "LSHIFT_EQUAL");     break;
			case RSHIFT_EQUAL:     fprintf(file, "RSHIFT_EQUAL");     break;
			case BIT_AND_EQUAL:    fprintf(file, "BIT_AND_EQUAL");     break;
			case BIT_OR_EQUAL:     fprintf(file, "BIT_OR_EQUAL");     break;
			case BIT_XOR_EQUAL:    fprintf(file, "BIT_XOR_EQUAL");     break;


			case INT_LITERAL:      fprintf(file, "INT_LITERAL = %d\n", tok->v.int_val);     break;
			case FLOAT_LITERAL:    fprintf(file, "FLOAT_LITERAL = %f\n", tok->v.float_val);     break;
			case DOUBLE_LITERAL:   fprintf(file, "DOUBLE_LITERAL = %f\n", tok->v.double_val);     break;
			case CHAR_LITERAL:     fprintf(file, "CHAR_LITERAL");     break;
			case STR_LITERAL:      fprintf(file, "STR_LITERAL");     break;


			case POUND:            fprintf(file, "POUND");  break;
			case EXP:              fprintf(file, "EXP");     break;

			default:
				fprintf(file, "Error, unknown token\n");
		}
	} else {
		switch (tok->type) {
			case END:              fprintf(file, "END");     break;
			case EQUALEQUAL:       fprintf(file, "==");     break;
			case GREATER:          fprintf(file, ">");     break;
			case GTEQ:             fprintf(file, ">=");     break;
			case LESS:             fprintf(file, "<");     break;
			case LTEQ:             fprintf(file, "<=");     break;
			case DOT:              fprintf(file, ".");     break;
			case NOTEQUAL:         fprintf(file, "!=");     break;
			case LOGICAL_OR:       fprintf(file, "||");     break;
			case LOGICAL_AND:      fprintf(file, "&&");     break;
			case LOGICAL_NEGATION: fprintf(file, "!");     break;
			case INT:              fprintf(file, "int");     break;
			case SHORT:            fprintf(file, "short");     break;
			case LONG:             fprintf(file, "long");     break;
			case FLOAT:            fprintf(file, "float");     break;
			case DOUBLE:           fprintf(file, "double");     break;
			case CHAR:             fprintf(file, "char");     break;
			case VOID:             fprintf(file, "void");     break;
			case SIGNED:           fprintf(file, "signed");     break;
			case UNSIGNED:         fprintf(file, "unsigned");     break;
			case DO:               fprintf(file, "do");     break;
			case FOR:              fprintf(file, "for");    break;
			case WHILE:            fprintf(file, "while");     break;
			case PRINT:            fprintf(file, "print");     break;
			case IF:               fprintf(file, "if");     break;
			case ELSE:             fprintf(file, "else");     break;
			case SWITCH:           fprintf(file, "switch");     break;
			case CASE:             fprintf(file, "case");     break;
			case DEFAULT:          fprintf(file, "default");     break;
			case CONTINUE:         fprintf(file, "continue");     break;
			case RETURN:           fprintf(file, "return");         break;
			case SIZEOF:           fprintf(file, "sizeof");         break;
			case BREAK:            fprintf(file, "break");     break;
			case GOTO:             fprintf(file, "goto");     break;
			case ID:               fprintf(file, "%s", tok->v.id);     break;
			case MOD:              fprintf(file, "%%");     break;
			case LPAREN:           fprintf(file, "(");     break;
			case RPAREN:           fprintf(file, ")");     break;
			case MULT:             fprintf(file, "*");     break;
			case ADD:              fprintf(file, "+");     break;
			case SUB:              fprintf(file, "-");     break;
			case DIV:              fprintf(file, "/");     break;
			case COLON:            fprintf(file, ":");     break;
			case SEMICOLON:        fprintf(file, ";");     break;
			case EQUAL:            fprintf(file, "=");     break;
			case COMMA:            fprintf(file, ",");     break;
			case LBRACKET:         fprintf(file, "[");     break;
			case RBRACKET:         fprintf(file, "]");     break;
			case LBRACE:           fprintf(file, "{");     break;
			case RBRACE:           fprintf(file, "}");     break;
			case INCREMENT:        fprintf(file, "++");    break;
			case DECREMENT:        fprintf(file, "--");    break;

			case LEFT_SHIFT:       fprintf(file, "<<");    break;
			case RIGHT_SHIFT:      fprintf(file, ">>");    break;
			case BIT_AND:          fprintf(file, "&");     break;
			case BIT_OR:           fprintf(file, "|");     break;
			case BIT_XOR:          fprintf(file, "^");     break;
			case BIT_NEGATION:     fprintf(file, "~");     break;

			case TERNARY:          fprintf(file, "?");     break;


			case ADDEQUAL:         fprintf(file, "+=");     break;
			case SUBEQUAL:         fprintf(file, "-=");     break;
			case MULTEQUAL:        fprintf(file, "*=");     break;
			case DIVEQUAL:         fprintf(file, "/=");     break;
			case MODEQUAL:         fprintf(file, "%%=");    break;
			case LSHIFT_EQUAL:     fprintf(file, "<<=");    break;
			case RSHIFT_EQUAL:     fprintf(file, ">>=");    break;
			case BIT_AND_EQUAL:    fprintf(file, "&=");     break;
			case BIT_OR_EQUAL:     fprintf(file, "|=");     break;
			case BIT_XOR_EQUAL:    fprintf(file, "^=");     break;

			case INT_LITERAL:      fprintf(file, "%d", tok->v.int_val);     break;
			case FLOAT_LITERAL:    fprintf(file, "%f", tok->v.float_val);     break;
			case DOUBLE_LITERAL:   fprintf(file, "%f", tok->v.double_val);     break;
			case CHAR_LITERAL:     fprintf(file, "'%s'", insert_char_esc_seq(tok->v.int_val, buf));     break;
			case STR_LITERAL:      fprintf(file, "\"%s\"", insert_esc_seqs(tok->v.id, buf));     break;

			case POUND:            fprintf(file, "#");  break;
			case EXP:              fprintf(file, "EXP");     break;

			default:
				fprintf(file, "Error, unknown token\n");
		}
	}
}


int print_token_to_str(token_value* tok, char* buf, size_t size)
{
	char buf2[MAX_TOKEN_LEN*2+1];
	switch (tok->type) {
		case END:              return snprintf(buf, size, "END");
		case EQUALEQUAL:       return snprintf(buf, size, "==");
		case GREATER:          return snprintf(buf, size, ">");
		case GTEQ:             return snprintf(buf, size, ">=");
		case LESS:             return snprintf(buf, size, "<");
		case LTEQ:             return snprintf(buf, size, "<=");
		case DOT:              return snprintf(buf, size, ".");
		case NOTEQUAL:         return snprintf(buf, size, "!=");
		case LOGICAL_OR:       return snprintf(buf, size, "||");
		case LOGICAL_AND:      return snprintf(buf, size, "&&");
		case LOGICAL_NEGATION: return snprintf(buf, size, "!");
		case INT:              return snprintf(buf, size, "int");
		case SHORT:            return snprintf(buf, size, "short");
		case LONG:             return snprintf(buf, size, "long");
		case FLOAT:            return snprintf(buf, size, "float");
		case DOUBLE:           return snprintf(buf, size, "double");
		case CHAR:             return snprintf(buf, size, "char");
		case VOID:             return snprintf(buf, size, "void");
		case SIGNED:           return snprintf(buf, size, "signed");
		case UNSIGNED:         return snprintf(buf, size, "unsigned");
		case DO:               return snprintf(buf, size, "do");
		case FOR:              return snprintf(buf, size, "for");
		case WHILE:            return snprintf(buf, size, "while");
		case PRINT:            return snprintf(buf, size, "print");
		case IF:               return snprintf(buf, size, "if");
		case ELSE:             return snprintf(buf, size, "else");
		case SWITCH:           return snprintf(buf, size, "switch");
		case CASE:             return snprintf(buf, size, "case");
		case DEFAULT:          return snprintf(buf, size, "default");
		case CONTINUE:         return snprintf(buf, size, "continue");
		case RETURN:           return snprintf(buf, size, "return");
		case BREAK:            return snprintf(buf, size, "break");
		case GOTO:             return snprintf(buf, size, "goto");
		case ID:               return snprintf(buf, size, "%s", tok->v.id);
		case MOD:              return snprintf(buf, size, "%%");
		case LPAREN:           return snprintf(buf, size, "(");
		case RPAREN:           return snprintf(buf, size, ")");
		case MULT:             return snprintf(buf, size, "*");
		case ADD:              return snprintf(buf, size, "+");
		case SUB:              return snprintf(buf, size, "-");
		case DIV:              return snprintf(buf, size, "/");
		case COLON:            return snprintf(buf, size, ":");
		case SEMICOLON:        return snprintf(buf, size, ";");
		case EQUAL:            return snprintf(buf, size, "=");
		case COMMA:            return snprintf(buf, size, ",");
		case LBRACKET:         return snprintf(buf, size, "[");
		case RBRACKET:         return snprintf(buf, size, "]");
		case LBRACE:           return snprintf(buf, size, "{");
		case RBRACE:           return snprintf(buf, size, "}");
		case INCREMENT:        return snprintf(buf, size, "++");
		case DECREMENT:        return snprintf(buf, size, "--");
		case LEFT_SHIFT:       return snprintf(buf, size, "<<");
		case RIGHT_SHIFT:      return snprintf(buf, size, ">>");
		case BIT_AND:          return snprintf(buf, size, "&");
		case BIT_OR:           return snprintf(buf, size, "|");
		case BIT_XOR:          return snprintf(buf, size, "^");
		case BIT_NEGATION:     return snprintf(buf, size, "~");

		case TERNARY:          return snprintf(buf, size, "?");


		case ADDEQUAL:         return snprintf(buf, size, "+=");
		case SUBEQUAL:         return snprintf(buf, size, "-=");
		case MULTEQUAL:        return snprintf(buf, size, "*=");
		case DIVEQUAL:         return snprintf(buf, size, "/=");
		case MODEQUAL:         return snprintf(buf, size, "%%=");
		case LSHIFT_EQUAL:     return snprintf(buf, size, "<<=");
		case RSHIFT_EQUAL:     return snprintf(buf, size, ">>=");
		case BIT_AND_EQUAL:    return snprintf(buf, size, "&=");
		case BIT_OR_EQUAL:     return snprintf(buf, size, "|=");
		case BIT_XOR_EQUAL:    return snprintf(buf, size, "^=");

		case INT_LITERAL:      return snprintf(buf, size, "%d", tok->v.int_val);
		case FLOAT_LITERAL:    return snprintf(buf, size, "%f", tok->v.float_val);
		case DOUBLE_LITERAL:   return snprintf(buf, size, "%f", tok->v.double_val);
		case CHAR_LITERAL:     return snprintf(buf, size, "'%s'", insert_char_esc_seq(tok->v.int_val, buf2));
		case STR_LITERAL:      return snprintf(buf, size, "\"%s\"", insert_esc_seqs(tok->v.id, buf2));

		case POUND:            return snprintf(buf, size, "#");
		case EXP:              return snprintf(buf, size, "EXP");

		default:
			fprintf(stderr, "Error, unknown token\n");
			return -1;
	}
}
