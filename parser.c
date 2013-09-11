#include "parser.h"

#include "c_utils.h"

#define CVECTOR_IMPLEMENTATION
#include "cvector.h"

#include <stdio.h>





void free_expression(expression* e)
{
	if (!e)
		return;

	if (e->tok.type == ID) {
		free(e->tok.v.id);
	} else if (e->tok.type == EXP) {
		free_expression(e->left);
	} else if (e->tok.type != INT_LITERAL && e->tok.type != FLOAT_LITERAL) {
		free_expression(e->left);
		free_expression(e->right);
	}
	free(e);
}

void free_statement(void* stmt)
{
	statement* s = (statement*)stmt;
	free(s->lvalue);
	free_expression(s->exp);
}


void free_var_value(void* var)
{
	var_value *v = var;

}

token_value read_token(FILE* file)
{
	static char token_buf[MAX_TOKEN_LEN];
	int c, i = 0;

	token_value tok;
	memset(&tok, 0, sizeof(token_value));

	do {
		c = getc(file);
	} while (isspace(c));

	//printf("getting token starting with '%c'\n", c);
	switch (c) {
	//TODO add compound assignments
	case '+': tok.type = ADD;  break;
	case '-': tok.type = SUB;  break;
	case '/': tok.type = DIV;  break;
	case '*': tok.type = MULT; break;
	case '=':
		if ((c = getc(file)) == '=') {
			tok.type = EQUALEQUAL;
		} else {
			ungetc(c, file);
			tok.type = EQUAL;
		}
		break;

	case ':': tok.type = COLON;     break;
	case ',': tok.type = COMMA;     break;
	case ';': tok.type = SEMICOLON; break;
	case '{': tok.type = LBRACE;    break;
	case '}': tok.type = RBRACE;    break;
	case '(': tok.type = LPAREN;    break;
	case ')': tok.type = RPAREN;    break;
	case '|':
		c = getc(file);
		if (c == '|')
			tok.type = LOGICAL_OR;
		else {
			parse_error("BITWISE_OR not supported yet\n");
			exit(0);
		}
		break;
		  	  
	case '&':
		c = getc(file);
		if (c == '&')
			tok.type = LOGICAL_AND;
		else {
			parse_error("BITWISE_AND not supported yet\n");
			exit(0);
		}
		break;

	case '<':
		c = getc(file);
		if (c == '=')
			tok.type = LTEQ;
		else if (c == '>')
			tok.type = NOTEQUAL;
		else {
			ungetc(c, file);
			tok.type = LESS;
		}
		break;

	case '>':
		c = getc(file);
		if (c == '=')
			tok.type = GTEQ;
		else {
			ungetc(c, file);
			tok.type = GREATER;
		}
		break;

	default:
		if (isdigit(c)) {
			while (isdigit(c)) {
				token_buf[i++] = c;
				c = getc(file);

				if (i == MAX_TOKEN_LEN-1)
					goto exit_error;
			}

			if (c == '.') {
				if (i == MAX_TOKEN_LEN-1)
					goto exit_error;

				token_buf[i++] = c;
				
				while (isdigit(c)) {
					token_buf[i++] = c;
					c = getc(file);

					if (i == MAX_TOKEN_LEN-1)
						goto exit_error;
				}

				ungetc(c, file);
				token_buf[i] = '\0';
				tok.type = FLOAT_LITERAL;
				tok.v.real = atof(token_buf);
			} else {
				ungetc(c, file);
				token_buf[i] = '\0';

				tok.type = INT_LITERAL;
				tok.v.integer = atoi(token_buf);
			}

		} else if (isalpha(c)) {
			while (isalnum(c)) {
				token_buf[i++] = c;
				c = getc(file);

				if (i == MAX_TOKEN_LEN-1)
					goto exit_error;
			}
			ungetc(c, file);
			token_buf[i] = '\0';

			if (!strcmp(token_buf, "do")) {         tok.type = DO;         break; }
			if (!strcmp(token_buf, "while")) {      tok.type = WHILE;      break; }
			if (!strcmp(token_buf, "if")) {         tok.type = IF;         break; }
			if (!strcmp(token_buf, "else")) {       tok.type = ELSE;       break; }
			if (!strcmp(token_buf, "int")) {        tok.type = INT;        break; }
			if (!strcmp(token_buf, "print")) {      tok.type = PRINT;      break; }
			if (!strcmp(token_buf, "goto")) {       tok.type = GOTO;       break; }
			if (!strcmp(token_buf, "int")) {        tok.type = INT;        break; }
			if (!strcmp(token_buf, "signed")) {     tok.type = SIGNED;     break; }
			if (!strcmp(token_buf, "unsigned")) {   tok.type = UNSIGNED;   break; }
			if (!strcmp(token_buf, "double")) {     tok.type = DOUBLE;     break; }
			if (!strcmp(token_buf, "float")) {      tok.type = FLOAT;      break; }

			/*
			if (c == ':') {
				getc(file);
				return LABEL;
			}
			*/

			tok.type = ID;
			tok.v.id = mystrdup(token_buf);
			assert(tok.v.id);

		} else if (c == EOF) {
	//		puts("EOF");
			tok.type = END;
		} else {
			perror("Error");
			tok.type = ERROR;
		}
	}

	return tok;

exit_error:
	parse_error("Token length is too long, max token length is \n"); // %d\n", MAX_TOKEN_LEN);
	exit(0);
}



void free_token_value(void* tok)
{
	token_value* t = tok;
	if (t->type == ID)
		free(t->v.id);
}


token_value* get_token(parsing_state* p)
{
	return GET_TOKEN_VAL(&p->tokens, p->pos++);
}

void parse_seek(int origin, long offset)
{

}




void parse_program(program_state* prog, FILE* file)
{
	parsing_state p;
	vec_void(&p.tokens, 0, 200, sizeof(token_value), free_token_value, NULL);
	token_value tok = read_token(file);
	while (tok.type != END && tok.type != ERROR) {
		push_void(&p.tokens, &tok);
		tok = read_token(file);
	}
	fclose(file);

	p.pos = 0;

	prog->pc = 0;
	vec_void(&prog->stmt_list, 0, 100, sizeof(statement), free_statement, NULL);
	vec_str(&prog->variables, 0, 20);
	vec_void(&prog->values, 0, 20, sizeof(var_value), free_var_value, NULL);

	//vec_i(&prog->values, 0, 20);

	var_decl(&p, prog);
	body(&p, prog);

	free_vec_void(&p.tokens);
}



void var_decl(parsing_state* p, program_state* prog)
{
	token_value* tok = get_token(p);
	var_type vtype = UNDECLARED;
	
	switch (tok->type) {
	case INT:
		vtype = INT_TYPE;
		break;
	case DOUBLE:
		vtype = DOUBLE_TYPE;
		break;
	default:	
		parse_error("Error parsing var_decl, INT expected.");
		exit(0);
	}

	id_list(p, prog, vtype);

	tok = get_token(p);
	if (tok->type != SEMICOLON) {
		parse_error("Error parsing var_decl, COMMA or SEMICOLON expected.");
		exit(0);
	}

}

void id_list(parsing_state* p, program_state* prog, var_type v_type)
{
	token_value* tok = get_token(p);
	var_value val;

	while (tok->type == ID) {
		push_str(&prog->variables, tok->v.id);
		
		//TODO for now initialize to 0 for the tests
		val.v.int_val = 0;
		val.type = v_type;
		push_void(&prog->values, &val);

		tok = get_token(p);
		if (tok->type != COMMA) {
			p->pos--;
			return;
		}

		tok = get_token(p);
	}

	parse_error("Error parsing id_list, ID expected.");
	exit(0);
}



void body(parsing_state* p, program_state* prog)
{

	token_value* tok = get_token(p);
	if (tok->type != LBRACE) {
		parse_error("Error parsing body, LBRACE expected.");
		exit(0);
	}

	statement_list(p, prog);

	tok = get_token(p);
	if (tok->type != RBRACE) {
		parse_error("Error parsing body, RBRACE expected.");
		exit(0);
	}
}



void statement_list(parsing_state* p, program_state* prog)
{
	token_value* tok;

	int go = 1;
	while (go) {
		tok = get_token(p);
		if (tok->type == END) break;

		switch (tok->type) {

		case ID:
			//could be label
			assign(p, prog);
			break;

		case WHILE:
			while_stmt(p, prog);
			break;

		case IF:
			if_stmt(p, prog);
			break;

		case PRINT:
			print_stmt(p, prog);
			break;

		case GOTO:
			goto_stmt(p, prog);
			break;

		default:
			p->pos--;
			go = 0;
		}
	}
}

void goto_stmt(parsing_state* p, program_state* prog)
{
	token_value* tok;

	tok = get_token(p);

	//label? number?
}


void assign(parsing_state* p, program_state* prog)
{
	token_value* tok = get_token(p);

	if (tok->type != EQUAL) {
		parse_error("Error parsing assignment, EQUAL expected.");
		exit(0);
	}

	statement an_assign;
	memset(&an_assign, 0, sizeof(statement));
	an_assign.type = ASSIGN_STMT;
	an_assign.lvalue = mystrdup(GET_TOKEN_VAL(&p->tokens, p->pos-2)->v.id);

	an_assign.exp = calloc(1, sizeof(expression));
	assert(an_assign.exp);

	cond_expr(p, prog, an_assign.exp);
	push_void(&prog->stmt_list, &an_assign);

	tok = get_token(p);
	if (tok->type != SEMICOLON) {
		parse_error("Error parsing assignment, SEMICOLON expected.\n");
		printf("%d\n", tok->type);
		exit(0);
	}
}

expression* copy_expr(expression* e)
{
	expression* copy = calloc(1, sizeof(expression));
	assert(copy);
	return memcpy(copy, e, sizeof(expression));
}
	
void cond_expr(parsing_state* p, program_state* prog, expression* e)
{
	logical_or_expr(p, prog, e);

	//TODO add ternary operator here
}

void logical_or_expr(parsing_state* p, program_state* prog, expression* e)
{
	logical_and_expr(p, prog, e);

	token_value* tok = get_token(p);
	while (tok->type == LOGICAL_OR) {
		e->left = copy_expr(e);
		e->tok.type = tok->type;

		e->right = calloc(1, sizeof(expression));
		assert(e->right);

		logical_and_expr(p, prog, e);
		tok = get_token(p);
	}
	p->pos--;
}


void logical_and_expr(parsing_state* p, program_state* prog, expression* e)
{
	equality_expr(p, prog, e);
	//skipping bitwise operators
}

void equality_expr(parsing_state* p, program_state* prog, expression* e)
{
	relational_expr(p, prog, e);

	token_value* tok = get_token(p);
	while (tok->type == EQUALEQUAL || tok->type == NOTEQUAL) {
		e->left = copy_expr(e);
		e->tok.type = tok->type;

		e->right = calloc(1, sizeof(expression));
		assert(e->right);

		relational_expr(p, prog, e->right);
		tok = get_token(p);
	}
	p->pos--;
}


void relational_expr(parsing_state* p, program_state* prog, expression* e)
{
	//skiping shift operators would be shift_expr() here
	//
	add_expr(p, prog, e);

	token_value* tok = get_token(p);
	while (tok->type == GREATER || tok->type == GTEQ || tok->type == LESS || tok->type == LTEQ) {
		e->left = copy_expr(e);
		e->tok.type = tok->type;

		e->right = calloc(1, sizeof(expression));
		assert(e->right);

		add_expr(p, prog, e->right);
		tok = get_token(p);
	}
	p->pos--;
}


/* expr -> term { '+'|'-' term } */
void add_expr(parsing_state* p, program_state* prog, expression* e)
{
	mult_expr(p, prog, e);

	token_value* tok = get_token(p);
	while (tok->type == ADD || tok->type == SUB) {
		e->left = copy_expr(e);
		e->tok.type = tok->type;

		e->right = calloc(1, sizeof(expression));
		assert(e->right);

		mult_expr(p, prog, e->right);
		tok = get_token(p);
	}
	p->pos--;
}

/* term -> factor { '*'|'/' factor } */
void mult_expr(parsing_state* p, program_state* prog, expression* e)
{
	token_value* tok;

	primary_expr(p, prog, e);
	tok = get_token(p);

	while (tok->type == MULT || tok->type == DIV || tok->type == MOD) {
		e->left = copy_expr(e);
		e->tok.type = tok->type;

		e->right = calloc(1, sizeof(expression));
		assert(e->right);

		primary_expr(p, prog, e->right);
		tok = get_token(p);
	}
	p->pos--;
}

/* factor -> '(' expr ')' | NUM */
void primary_expr(parsing_state* p, program_state* prog, expression* e)
{
	token_value* tok = get_token(p);

	switch (tok->type) {
	case LPAREN:
		e->tok.type = EXP;
		e->left = calloc(1, sizeof(expression));
		assert(e->left);
		cond_expr(p, prog, e->left);

		tok = get_token(p);
		if (tok->type != RPAREN) {
			parse_error("Error parsing primary_expr, RPAREN expected.\n");
			exit(0);
		}
		break;
	case ID:
		e->tok.type = ID;
		e->tok.v.id = mystrdup(tok->v.id);
		break;
	case INT_LITERAL:
		e->tok.type = INT_LITERAL;
		e->tok.v.integer = tok->v.integer;
		break;
	case FLOAT_LITERAL:
		e->tok.type = FLOAT_LITERAL;
		e->tok.v.real = tok->v.real;
		break;
	default:
		parse_error("Error parsing primary_expr, LPAREN or literal expected\n");
		exit(0);
	}
}		



void if_stmt(parsing_state* p, program_state* prog)
{
	statement an_if;
	memset(&an_if, 0, sizeof(statement));
	an_if.type = IF_STMT;

	an_if.exp = calloc(1, sizeof(expression));
	assert(an_if.exp);

	cond_expr(p, prog, an_if.exp);

	size_t if_loc = prog->stmt_list.size;
	push_void(&prog->stmt_list, &an_if);

	body(p, prog);

	GET_STMT(&prog->stmt_list, if_loc)->jump_to = prog->stmt_list.size;
}


void print_stmt(parsing_state* p, program_state* prog)
{
	token_value* tok = get_token(p);

	if (tok->type != ID) {
		parse_error("Error parsing print_stmt, ID expected.");
		exit(0);
	}

	statement a_print;
	memset(&a_print, 0, sizeof(statement));
	a_print.type = PRINT_STMT;

	a_print.lvalue = mystrdup(tok->v.id);
	
	tok = get_token(p);
	if (tok->type != SEMICOLON) {
		parse_error("Error parsing print_stmt, SEMICOLON expected.");
		exit(0);
	}

	push_void(&prog->stmt_list, &a_print);
}



void while_stmt(parsing_state* p, program_state* prog)
{
	statement a_while;
	memset(&a_while, 0, sizeof(statement));
	a_while.type = WHILE_STMT;

	a_while.exp = calloc(1, sizeof(expression));
	assert(a_while.exp);

	cond_expr(p, prog, a_while.exp);

	size_t while_loc = prog->stmt_list.size;
	push_void(&prog->stmt_list, &a_while);

	body(p, prog);

	statement a_goto;
	memset(&a_goto, 0, sizeof(statement));
	a_goto.type = GOTO_STMT;
	a_goto.jump_to = while_loc;
	push_void(&prog->stmt_list, &a_goto);

	GET_STMT(&prog->stmt_list, while_loc)->jump_to = prog->stmt_list.size;
}



void parse_error(const char *str)
{
	fprintf(stderr, "%s", str);
}
