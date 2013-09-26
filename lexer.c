
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


token_lex read_token(FILE* file, lexer_state* lex_state)
{
	static char token_buf[MAX_TOKEN_LEN];
	int c, i = 0, tmp;

	token_lex tok_lex;
	memset(&tok_lex, 0, sizeof(token_value));

start:

	do {
		c = getc(file);
		if (c == '\n') {
			lex_state->cur_pos = 0;
			lex_state->cur_tok = 0;
			lex_state->cur_line++;
		}
		lex_state->cur_pos++;
	} while (isspace(c));

	lex_state->cur_tok++;

	tok_lex.pos = lex_state->cur_pos - 1;
	tok_lex.line = lex_state->cur_line;

//	printf("getting token starting with '%c'\n", c);
	switch (c) {
	case ':': tok_lex.tok.type = COLON;     break;
	case ',': tok_lex.tok.type = COMMA;     break;
	case ';': tok_lex.tok.type = SEMICOLON; break;
	case '{': tok_lex.tok.type = LBRACE;    break;
	case '}': tok_lex.tok.type = RBRACE;    break;
	case '(': tok_lex.tok.type = LPAREN;    break;
	case ')': tok_lex.tok.type = RPAREN;    break;
	case '?': tok_lex.tok.type = TERNARY;   break;

	//only used in preprocessor
	case '#': tok_lex.tok.type = POUND;	    break;

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
				if (c == '\n')
					break;
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
		} else {
			fprintf(stderr, "Error: BITWISE_OR not supported yet\n");
			exit(0);
		}
		break;
		  	  
	case '&':
		HANDLE_BACKSLASH();
		if (c == '&') {
			tok_lex.tok.type = LOGICAL_AND;
		} else {
			fprintf(stderr, "Error: BITWISE_AND not supported yet\n");
			exit(0);
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
		} else {
			ungetc(c, file);
			lex_state->cur_pos--;
			tok_lex.tok.type = GREATER;
		}
		break;

	case '\'':
		HANDLE_BACKSLASH(); //currently precludes escape characters, TODO
		tmp = c; //save character
		HANDLE_BACKSLASH();
		if (c != '\'') {
			fprintf(stderr, "Error: multi-character character constant\n");
			exit(0);
		}
		tok_lex.tok.type = INT_LITERAL;
		tok_lex.tok.v.int_val = tmp;
		break;

	default:
		if (isdigit(c)) {
			while (isdigit(c)) {
				token_buf[i++] = c;
				HANDLE_BACKSLASH();

				if (i == MAX_TOKEN_LEN-1)
					goto token_length_error;
			}

			if (c == '.') {
				if (i == MAX_TOKEN_LEN-1)
					goto token_length_error;

				token_buf[i++] = c;
				HANDLE_BACKSLASH();
				
				while (isdigit(c)) {
					token_buf[i++] = c;
					HANDLE_BACKSLASH();

					if (i == MAX_TOKEN_LEN-1)
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

				if (i == MAX_TOKEN_LEN-1)
					goto token_length_error;
			}
			ungetc(c, file);
			lex_state->cur_pos--;
			token_buf[i] = '\0';

			//should turn these off for preprocessor ... thoughI think you deserve problems
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

		} else if (c == EOF) {
			tok_lex.tok.type = END;
		} else {
			fprintf(stderr, "Scanning Error");
			tok_lex.tok.type = ERROR;
		}
	}

	return tok_lex;

stray_backslash:
	fprintf(stderr, "Error: stray \\ in program (perhaps you have space between it and a newline)\n");
	exit(0);

token_length_error:
	fprintf(stderr, "Error: Token length is too long, max token length is %d\n", MAX_TOKEN_LEN);
	exit(0);
}




void print_token(token_value* tok, FILE* file, int print_enum)
{
	if (print_enum) { 
		switch (tok->type) {
			case ERROR:            fprintf(file, "ERROR");     break;
			case END:              fprintf(file, "END");     break;
			case EQUALEQUAL:       fprintf(file, "EQUALEQUAL");     break;
			case GREATER:          fprintf(file, "GREATER");     break;
			case GTEQ:             fprintf(file, "GTEQ");     break;
			case LESS:             fprintf(file, "LESS");     break;
			case LTEQ:             fprintf(file, "LTEQ");     break;
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
			case TERNARY:          fprintf(file, "TERNARY");     break;


			case ADDEQUAL:         fprintf(file, "ADDEQUAL");     break;
			case SUBEQUAL:         fprintf(file, "SUBEQUAL");     break;
			case MULTEQUAL:        fprintf(file, "MULTEQUAL");     break;
			case DIVEQUAL:         fprintf(file, "DIVEQUAL");     break;
			case MODEQUAL:         fprintf(file, "MODEQUAL");     break;
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
			case ERROR:            fprintf(file, "ERROR");     break;
			case END:              fprintf(file, "END");     break;
			case EQUALEQUAL:       fprintf(file, "==");     break;
			case GREATER:          fprintf(file, ">");     break;
			case GTEQ:             fprintf(file, ">=");     break;
			case LESS:             fprintf(file, "<");     break;
			case LTEQ:             fprintf(file, "<=");     break;
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
			case INCREMENT:        fprintf(file, "++");     break;
			case DECREMENT:        fprintf(file, "--");     break;
			case TERNARY:          fprintf(file, "?");     break;


			case ADDEQUAL:         fprintf(file, "+=");     break;
			case SUBEQUAL:         fprintf(file, "-=");     break;
			case MULTEQUAL:        fprintf(file, "*=");     break;
			case DIVEQUAL:         fprintf(file, "/=");     break;
			case MODEQUAL:         fprintf(file, "%%=");     break;
			case INT_LITERAL:      fprintf(file, "%d", tok->v.int_val);     break;
			case FLOAT_LITERAL:    fprintf(file, "%f", tok->v.float_val);     break;
			case DOUBLE_LITERAL:   fprintf(file, "%f", tok->v.double_val);     break;
			case CHAR_LITERAL:     fprintf(file, "'%c'", tok->v.int_val);     break;
			case STR_LITERAL:      fprintf(file, "%s", tok->v.id);     break;

			case POUND:            fprintf(file, "#");  break;
			case EXP:              fprintf(file, "EXP");     break;

			default:
				fprintf(file, "Error, unknown token\n");
		}
	}
}


int print_token_to_str(token_value* tok, char* buf, size_t size)
{
	switch (tok->type) {
		case ERROR:            return snprintf(buf, size, "ERROR");
		case END:              return snprintf(buf, size, "END");
		case EQUALEQUAL:       return snprintf(buf, size, "==");
		case GREATER:          return snprintf(buf, size, ">");
		case GTEQ:             return snprintf(buf, size, ">=");
		case LESS:             return snprintf(buf, size, "<");
		case LTEQ:             return snprintf(buf, size, "<=");
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
		case TERNARY:          return snprintf(buf, size, "?");


		case ADDEQUAL:         return snprintf(buf, size, "+=");
		case SUBEQUAL:         return snprintf(buf, size, "-=");
		case MULTEQUAL:        return snprintf(buf, size, "*=");
		case DIVEQUAL:         return snprintf(buf, size, "/=");
		case MODEQUAL:         return snprintf(buf, size, "%%=");
		case INT_LITERAL:      return snprintf(buf, size, "%d", tok->v.int_val);
		case FLOAT_LITERAL:    return snprintf(buf, size, "%f", tok->v.float_val);
		case DOUBLE_LITERAL:   return snprintf(buf, size, "%f", tok->v.double_val);
		case CHAR_LITERAL:     return snprintf(buf, size, "'%c'", tok->v.int_val);
		case STR_LITERAL:      return snprintf(buf, size, "%s", tok->v.id);

		case POUND:            return snprintf(buf, size, "#");
		case EXP:              return snprintf(buf, size, "EXP");

		default:
			fprintf(stderr, "Error, unknown token\n");
			return -1;
	}
}
