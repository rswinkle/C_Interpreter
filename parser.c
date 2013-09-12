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
	case ':': tok.type = COLON;     break;
	case ',': tok.type = COMMA;     break;
	case ';': tok.type = SEMICOLON; break;
	case '{': tok.type = LBRACE;    break;
	case '}': tok.type = RBRACE;    break;
	case '(': tok.type = LPAREN;    break;
	case ')': tok.type = RPAREN;    break;

	case '+':
		c = getc(file);
		if (c == '=') {
			tok.type = ADDEQUAL;
		} else {
			ungetc(c, file);
			tok.type = ADD;
		}
		break;

	case '-':
		c = getc(file);
		if (c == '=') {
			tok.type = SUBEQUAL;
		} else {
			ungetc(c, file);
			tok.type = SUB;
		}
		break;
		
	case '*':
		c = getc(file);
		if (c == '=') {
			tok.type = MULTEQUAL;
		} else {
			ungetc(c, file);
			tok.type = MULT;
		}
		break;

	case '/':
		c = getc(file);
		if (c == '=') {
			tok.type = DIVEQUAL;
		} else {
			ungetc(c, file);
			tok.type = DIV;
		}
		break;

	case '%':
		c = getc(file);
		if (c == '=') {
			tok.type = MODEQUAL;
		} else {
			ungetc(c, file);
			tok.type = MOD;
		}
		break;

	case '=':
		if ((c = getc(file)) == '=') {
			tok.type = EQUALEQUAL;
		} else {
			ungetc(c, file);
			tok.type = EQUAL;
		}
		break;

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
			perror("Scanning Error");
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


void print_token(token_value* tok)
{
	switch (tok->type) {
		case ERROR:            puts("ERROR");     break;
		case END:              puts("END");     break;
		case EQUALEQUAL:       puts("EQUALEQUAL");     break;
		case GREATER:          puts("GREATER");     break;
		case GTEQ:             puts("GTEQ");     break;
		case LESS:             puts("LESS");     break;
		case LTEQ:             puts("LTEQ");     break;
		case NOTEQUAL:         puts("NOTEQUAL");     break;
		case LOGICAL_OR:       puts("LOGICAL_OR");     break;
		case LOGICAL_AND:      puts("LOGICAL_AND");     break;
		case INT:              puts("INT");     break;
		case SHORT:            puts("SHORT");     break;
		case LONG:             puts("LONG");     break;
		case FLOAT:            puts("FLOAT");     break;
		case DOUBLE:           puts("DOUBLE");     break;
		case CHAR:             puts("CHAR");     break;
		case VOID:             puts("VOID");     break;
		case SIGNED:           puts("SIGNED");     break;
		case UNSIGNED:         puts("UNSIGNED");     break;
		case DO:               puts("DO");     break;
		case WHILE:            puts("WHILE");     break;
		case PRINT:            puts("PRINT");     break;
		case IF:               puts("IF");     break;
		case ELSE:             puts("ELSE");     break;
		case SWITCH:           puts("SWITCH");     break;
		case CASE:             puts("CASE");     break;
		case DEFAULT:          puts("DEFAULT");     break;
		case CONTINUE:         puts("CONTINUE");     break;
		case BREAK:            puts("BREAK");     break;
		case GOTO:             puts("GOTO");     break;
		case ID:               printf("ID = %s\n", tok->v.id);     break;
		case MOD:              puts("MOD");     break;
		case LPAREN:           puts("LPAREN");     break;
		case RPAREN:           puts("RPAREN");     break;
		case MULT:             puts("MULT");     break;
		case ADD:              puts("ADD");     break;
		case SUB:              puts("SUB");     break;
		case DIV:              puts("DIV");     break;
		case COLON:            puts("COLON");     break;
		case SEMICOLON:        puts("SEMICOLON");     break;
		case EQUAL:            puts("EQUAL");     break;
		case COMMA:            puts("COMMA");     break;
		case LBRACKET:         puts("LBRACKET");     break;
		case RBRACKET:         puts("RBRACKET");     break;
		case LBRACE:           puts("LBRACE");     break;
		case RBRACE:           puts("RBRACE");     break;
		case ADDEQUAL:         puts("ADDEQUAL");     break;
		case SUBEQUAL:         puts("SUBEQUAL");     break;
		case MULTEQUAL:        puts("MULTEQUAL");     break;
		case DIVEQUAL:         puts("DIVEQUAL");     break;
		case MODEQUAL:         puts("MODEQUAL");     break;
		case INT_LITERAL:      printf("INT_LITERAL = %d\n", tok->v.integer);     break;
		case FLOAT_LITERAL:    puts("FLOAT_LITERAL");     break;
		case CHAR_LITERAL:     puts("CHAR_LITERAL");     break;
		case STR_LITERAL:      puts("STR_LITERAL");     break;
		case LABEL:            puts("LABEL");     break;
		case EXP:              puts("EXP");     break;

		default:
			puts("Error, unknown token\n");
	}
}


token_value* get_token(parsing_state* p)
{
	token_value* tok = GET_TOKEN_VAL(&p->tokens, p->pos++);
//	print_token(tok);
	return tok;

	//return GET_TOKEN_VAL(&p->tokens, p->pos++);
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
	vec_str(&prog->global_variables, 0, 20);
	vec_void(&prog->global_values, 0, 20, sizeof(var_value), free_var_value, NULL);

	// put this in a loop?  when body changes to function
	declaration_list(&p, prog);
	body(&p, prog);

	free_vec_void(&p.tokens);
}


/* declaration_list -> declaration { declaration } */
void declaration_list(parsing_state* p, program_state* prog)
{
	declaration(p, prog);

	var_type vtype = declaration_specifier(p, prog);
	while (vtype != UNKNOWN) {
		p->pos--;
		declaration(p, prog);

		vtype = declaration_specifier(p, prog);
	}

	p->pos--;
}

/* declaration -> declaration_specifier initialized_declarator_list ';' */
void declaration(parsing_state* p, program_state* prog)
{
	var_type vtype = declaration_specifier(p, prog);
	if (vtype == UNKNOWN) {
		p->pos--;
		return;
	}

	initialized_declarator_list(p, prog, vtype);

	token_value* tok = get_token(p);
	if (tok->type != SEMICOLON) {
		parse_error("Error parsing var_decl, COMMA or SEMICOLON expected.");
		exit(0);
	}
}

var_type declaration_specifier(parsing_state* p, program_state* prog)
{
	token_value* tok = get_token(p);
	
	switch (tok->type) {
	case INT:
		return INT_TYPE;
		break;
	case DOUBLE:
		return DOUBLE_TYPE;
		break;
	}
	
	return UNKNOWN;
}


/* initialized_declarator_list -> initialized_declarator { , initialized_declarator } */
void initialized_declarator_list(parsing_state* p, program_state* prog, var_type v_type)
{
	initialized_declarator(p, prog, v_type);

	token_value* tok = get_token(p);
	while (tok->type == COMMA) {

		initialized_declarator(p, prog, v_type);
		tok = get_token(p);
	}
	p->pos--;
}



/* initialized_declarator -> declarator [ = initializer ]
 *                        -> ID [ = assign_expr ]
 */
void initialized_declarator(parsing_state* p, program_state* prog, var_type v_type)
{
	token_value* tok = get_token(p);
	if (tok->type != ID) {
		parse_error("Error parsing initialized_declarator, ID expected.\n");
		exit(0);
	}

	//TODO finish working this in with above and make declaration_specifiers (aka type) function

	push_str(&prog->global_variables, tok->v.id);
	
	//TODO for now initialize to 0 for the tests
	var_value val;
	val.v.int_val = 0;
	val.type = v_type;
	push_void(&prog->global_values, &val);

	tok = get_token(p);
	if (tok->type == EQUAL) {
		p->pos -= 2;  //put ID = back for assign_expr

		statement an_expr;
		memset(&an_expr, 0, sizeof(statement));
		an_expr.type = EXPR_STMT;

		an_expr.exp = calloc(1, sizeof(expression));
		assert(an_expr.exp);
		assign_expr(p, prog, an_expr.exp);

		push_void(&prog->stmt_list, &an_expr);
	}

	p->pos--;
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
			p->pos--;
			expression_stmt(p, prog);
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


void expression_stmt(parsing_state* p, program_state* prog)
{
	statement an_expr;
	memset(&an_expr, 0, sizeof(statement));

	an_expr.exp = calloc(1, sizeof(expression));
	assert(an_expr.exp);

	an_expr.type = EXPR_STMT;

	expr(p, prog, an_expr.exp);

	push_void(&prog->stmt_list, &an_expr);


	token_value* tok = get_token(p);
	if (tok->type != SEMICOLON) {
		parse_error("Error parsing expression_stmt, SEMICOLON expected\n");
		exit(0);
	}
}

void expr(parsing_state* p, program_state* prog, expression* e)
{
	comma_expr(p, prog, e);
}

void comma_expr(parsing_state* p, program_state* prog, expression* e)
{
	assign_expr(p, prog, e);

	token_value* tok = get_token(p);
	while (tok->type == COMMA) {
		e->left = copy_expr(e);
		e->tok.type = tok->type;

		e->right = calloc(1, sizeof(expression));
		assert(e->right);

		assign_expr(p, prog, e->right);
		tok = get_token(p);
	}
	p->pos--;
}


int assignment_operator(Token tok)
{
	return (tok == EQUAL || tok == ADDEQUAL || tok == SUBEQUAL ||
	        tok == MULTEQUAL || tok == DIVEQUAL || tok == MODEQUAL);
}

/* assign_expr -> cond_expr | unary_expr assign_op assign_expr */
void assign_expr(parsing_state* p, program_state* prog, expression* e)
{
	token_value* tok = get_token(p);
	
	if (tok->type != ID) {
		p->pos--;
		cond_expr(p, prog, e);

		tok = get_token(p);
		if (assignment_operator(tok->type)) {
			if (e->tok.type != ID) {  //TODO for now
				parse_error("Error: lvalue required as left operand of assignment.\n");	
				exit(0);
			}
		}
		p->pos--;
		return;
	}

	char* id = tok->v.id;
	
	tok = get_token(p);

	if (!assignment_operator(tok->type)) {
		p->pos -= 2;
		cond_expr(p, prog, e);

		tok = get_token(p);
		if (assignment_operator(tok->type)) {
			if (e->tok.type != ID) {  //TODO for now
				parse_error("Error: lvalue required as left operand of assignment.\n");	
				exit(0);
			}
		}
		p->pos--;
		return;
	}

	e->tok.type = ID;
	e->tok.v.id = mystrdup(id);
	
	//assignment operators are right associative for obvious reasons
	//they're the only right associative binary operators in C
	//
	//well technically the comma is fully associative semantically
	//
	//strangely this looks like the same left associative code for other
	//operators.  The difference is the others aren't recursive
	//just loop always modifying the same top level expression
	//copying it into the left side (the recursion) as needed.
	//
	//Here the top level left side never changes and only the right
	//side is expanded in the recursive call
	e->left = copy_expr(e);
	e->tok.type = tok->type;

	e->right = calloc(1, sizeof(expression));
	assert(e->right);

	assign_expr(p, prog, e->right);
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

/* logical_or_expr -> logical_and_expr { '||' logical_and_expr } */
void logical_or_expr(parsing_state* p, program_state* prog, expression* e)
{
	logical_and_expr(p, prog, e);

	token_value* tok = get_token(p);
	while (tok->type == LOGICAL_OR) {
		e->left = copy_expr(e);
		e->tok.type = tok->type;

		e->right = calloc(1, sizeof(expression));
		assert(e->right);

		logical_and_expr(p, prog, e->right);
		tok = get_token(p);
	}
	p->pos--;
}


/* logical_and_expr -> bitwise_or_expr
 *                     logical_and_expr '&&' bitwise_or_expr
 * logical_and_expr -> equality_expr { '&&' equality_expr }
 */
void logical_and_expr(parsing_state* p, program_state* prog, expression* e)
{
	equality_expr(p, prog, e);

	token_value* tok = get_token(p);
	while (tok->type == LOGICAL_AND) {
		e->left = copy_expr(e);
		e->tok.type = tok->type;

		e->right = calloc(1, sizeof(expression));
		assert(e->right);

		equality_expr(p, prog, e->right);
		tok = get_token(p);
	}
	p->pos--;
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
		expr(p, prog, e->left);

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
		if (tok->type == ADDEQUAL)
			puts("plusequal");
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
