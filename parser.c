#include "parser.h"

#include "c_utils.h"

#define CVECTOR_IMPLEMENTATION
#include "cvector.h"

#include <stdio.h>


int num_value;
char token_buf[MAX_TOKEN_LEN];

FILE* file;
vector_void stmt_list;

vector_str variables;
vector_i values;


void free_statement(void* stmt)
{
	statement* s = (statement*)stmt;
	free(s->lvalue);
	free(s->left.id);
	free(s->right.id);
}


Token get_token()
{
	int c, i = 0;

	do {
		c = getc(file);
	} while (isspace(c));

	switch (c) {
	case '+': return PLUS;
	case '-': return MINUS;
	case '/': return DIV;
	case '*': return MULT;
	case '=': return EQUAL;
	case ':': return COLON;
	case ',': return COMMA;
	case ';': return SEMICOLON;
	case '{': return LBRACE;
	case '}': return RBRACE;
	case '<':
		c = getc(file);
		if (c == '=')
			return LTEQ;
		else if (c == '>')
			return NOTEQUAL;
		else {
			ungetc(c, file);
			return LESS;
		}
	case '>':
		c = getc(file);
		if (c == '=')
			return GTEQ;
		else {
			ungetc(c, file);
			return GREATER;
		}

	default :
		if (isdigit(c)) {
			while (isdigit(c)) {
				token_buf[i++] = c;
				c = getc(file);
			}
			ungetc(c, file);
			token_buf[i] = '\0';
			num_value = atoi(token_buf);
		//	printf("token_buf = %s\t num_value = %d\n", token_buf, num_value);

			return NUM;

		} else if (isalpha(c)) {
			while (isalnum(c)) {
				token_buf[i++] = c;
				c = getc(file);
			}
			ungetc(c, file);
			token_buf[i] = '\0';

			if (!strcmp(token_buf, "WHILE")) return WHILE;
			if (!strcmp(token_buf, "IF")) return IF;
			if (!strcmp(token_buf, "VAR")) return VAR;
			if (!strcmp(token_buf, "PRINT") || !strcmp(token_buf, "print")) return PRINT;

			return ID;
		}
		else if (c == EOF)
			return END;
		else
			return ERROR;
	}
}


void parse_program()
{
	vec_str(&variables, 0, 20);
	vec_i(&values, 0, 20);
	var_decl();
	body();

}



void var_decl()
{
	Token tok = get_token();
	if (tok != VAR) {
		parse_error("Error parsing var_decl, VAR expected.");
		exit(0);
	}

	id_list();
}

void id_list()
{
	Token tok;

	tok = get_token();
	if (tok != ID) {
		parse_error("Error parsing id_list, ID expected.");
		exit(0);
	}

	while (tok == ID) {
		push_str(&variables, token_buf);
		push_i(&values, 0);

		tok = get_token();
		if (tok != COMMA) {
			if (tok != SEMICOLON) {
				parse_error("Error parsing id_list, COMMA or SEMICOLON expected.");
				exit(0);
			}
			return;
		}

		tok = get_token();
	}

	parse_error("Error parsing id_list, ID expected.");
	exit(0);
}



void body()
{
	Token tok;

	tok = get_token();
	if (tok != LBRACE) {
		parse_error("Error parsing body, LBRACE expected.");
		exit(0);
	}

	statement_list();
}



void statement_list()
{
	Token tok;

	int go = 1;
	while (go) {
		tok = get_token();
		if (tok == END) break;

		switch (tok) {

		case ID:
			assign();
			break;

		case WHILE:
			while_stmt();
			break;

		case IF:
			if_stmt();
			break;

		case PRINT:
			print_stmt();
			break;

		case RBRACE:
			go = 0;
			break;

		default:
			parse_error("Error parsing body, ID, WHILE, IF, PRINT or RBRACE expected.");
			exit(0);
		}
	}
}


void assign()
{
	Token tok = get_token();

	if (tok != EQUAL) {
		parse_error("Error parsing assignment, EQUAL expected.");
		exit(0);
	}

	statement an_assign;
	memset(&an_assign, 0, sizeof(statement));
	an_assign.type = ASSIGN_STMT;
	an_assign.lvalue = mystrdup(token_buf);

	expr(&an_assign);
	push_void(&stmt_list, &an_assign);
}

void expr(statement* stmt)
{
	Token tok = get_token();
	if (tok == ID) {
		stmt->left.type = ID;
		stmt->left.id = mystrdup(token_buf);
	} else if (tok == NUM) {
		stmt->left.type = NUM;
		stmt->left.num = num_value;
	} else {
		parse_error("Error parsing expression, ID or NUM expected.");
		exit(0);
	}

	tok = get_token();
	if (tok == PLUS || tok == MINUS || tok == MULT || tok == DIV)
		stmt->op = tok;
	else if (tok == SEMICOLON) {
		stmt->op = tok;
		return;
	} else {
		parse_error("Error parsing expression, PLUS, MINUS, MULT, DIV or SEMICOLON expected.");
		exit(0);
	}

	tok = get_token();
	if (tok == ID) {
		stmt->right.type = ID;
		stmt->right.id = mystrdup(token_buf);
	} else if (tok == NUM) {
		stmt->right.type = NUM;
		stmt->right.num = num_value;
	} else {
		parse_error("Error parsing expression, ID or NUM expected.");
		exit(0);
	}

	tok = get_token();
	if (tok != SEMICOLON) {
		parse_error("Error parsing assignment, SEMICOLON expected.");
		exit(0);
	}
}


void if_stmt()
{
	statement an_if;
	memset(&an_if, 0, sizeof(statement));
	an_if.type = IF_STMT;

	p_condition(&an_if);
	int if_loc = stmt_list.size;
	push_void(&stmt_list, &an_if);

	body();

	GET_STMT(&stmt_list, if_loc)->jump_to = stmt_list.size;
}


void p_condition(statement* stmt)
{
	Token tok = get_token();
	if (tok == ID) {
		stmt->left.type = ID;
		stmt->left.id = mystrdup(token_buf);
	} else if (tok == NUM) {
		stmt->left.type = NUM;
		stmt->left.num = num_value;
	} else {
		parse_error("Error parsing condition, ID or NUM expected.");
		exit(0);
	}

	tok = get_token();
	if (tok == GREATER || tok == LESS || tok == GTEQ || tok == LTEQ || tok == NOTEQUAL)
		stmt->op = tok;
	else {
		parse_error("Error parsing condition, relop expected.");
		exit(0);
	}

	tok = get_token();
	if (tok == ID) {
		stmt->right.type = ID;
		stmt->right.id = mystrdup(token_buf);
	} else if (tok == NUM) {
		stmt->right.type = NUM;
		stmt->right.num = num_value;
	} else {
		parse_error("Error parsing expression, ID or NUM expected.");
		exit(0);
	}
}




void print_stmt()
{
	Token tok = get_token();

	if (tok != ID) {
		parse_error("Error parsing print_stmt, ID expected.");
		exit(0);
	}

	statement a_print;
	memset(&a_print, 0, sizeof(statement));
	a_print.type = PRINT_STMT;

	a_print.lvalue = mystrdup(token_buf);

	if ((tok = get_token()) != SEMICOLON) {
		parse_error("Error parsing print_stmt, SEMICOLON expected.");
		exit(0);
	}

	push_void(&stmt_list, &a_print);
}



void while_stmt()
{
	statement a_while;
	memset(&a_while, 0, sizeof(statement));
	a_while.type = WHILE_STMT;

	p_condition(&a_while);

	int while_loc = stmt_list.size;
	push_void(&stmt_list, &a_while);

	body();

	statement a_goto;
	memset(&a_goto, 0, sizeof(statement));
	a_goto.type = GOTO_STMT;
	a_goto.jump_to = while_loc;
	push_void(&stmt_list, &a_goto);

	GET_STMT(&stmt_list, while_loc)->jump_to = stmt_list.size;
}



void parse_error(const char *str)
{
	fprintf(stderr, "%s", str);
}
