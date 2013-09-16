#include "parser.h"

#include "c_utils.h"

#define CVECTOR_IMPLEMENTATION
#include "cvector.h"

#include <stdio.h>




unsigned int make_expression(program_state* prog)
{
	expression e = { 0 };
	push_void(&prog->expressions, &e);
	return prog->expressions.size - 1;
}
	

void free_expression(void* expr)
{
	//if (!e)
	//	return;

	expression* e = expr;
	if (e->tok.type == ID) {
		free(e->tok.v.id);
	}
	/*
	} else if (e->tok.type == EXP) {
		free_expression(GET_EXPRESSION(&prog->expressions, e->left));
	} else if (e->tok.type != INT_LITERAL && e->tok.type != FLOAT_LITERAL) {
		free_expression(GET_EXPRESSION(&prog->expressions, e->left));
		free_expression(GET_EXPRESSION(&prog->expressions, e->right));
	}
	*/
	//free(e);
}

void free_var_list(void* l)
{
	list_head* head = l;
	var_value* p, *tmp;

	list_for_each_entry_safe(p, tmp, var_value, head, list) {
		list_del(&p->list);
		free(p);
	}
}

void free_variable(void* var)
{
	variable* v = var;
	free(v->name);
	free_var_list(&v->head);
}


void init_function(void* to, void* from)
{
	function* to_func = to, *from_func = from;

	//to_func->ret;
	to_func->pc = 0;
	vec_void(&to_func->stmt_list, 0, 100, sizeof(statement), free_statement, NULL);
	vec_void(&to_func->variables, 0, 20, sizeof(variable), free_variable, NULL);
}

void free_function(void* func)
{
	function* f = func;
	free_vec_void(&f->stmt_list);
	free_vec_void(&f->variables);
}

void free_statement(void* stmt)
{
	statement* s = (statement*)stmt;
	free(s->lvalue);
}


void free_var_value(void* var)
{
	var_value *v = var;
	//TODO
	//nothing to free for now

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
			parse_error(&tok, "BITWISE_OR not supported yet\n");
			exit(0);
		}
		break;
		  	  
	case '&':
		c = getc(file);
		if (c == '&')
			tok.type = LOGICAL_AND;
		else {
			parse_error(&tok, "BITWISE_AND not supported yet\n");
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
			if (!strcmp(token_buf, "return")) {     tok.type = RETURN;     break; }
			if (!strcmp(token_buf, "switch")) {     tok.type = SWITCH;     break; }
			if (!strcmp(token_buf, "break")) {      tok.type = BREAK;      break; }

			if (!strcmp(token_buf, "case")) {       tok.type = CASE;       break; }
			if (!strcmp(token_buf, "default")) {    tok.type = DEFAULT;    break; }
			if (!strcmp(token_buf, "continue")) {   tok.type = CONTINUE;   break; }

			if (!strcmp(token_buf, "int")) {        tok.type = INT;        break; }
			if (!strcmp(token_buf, "signed")) {     tok.type = SIGNED;     break; }
			if (!strcmp(token_buf, "unsigned")) {   tok.type = UNSIGNED;   break; }
			if (!strcmp(token_buf, "double")) {     tok.type = DOUBLE;     break; }
			if (!strcmp(token_buf, "float")) {      tok.type = FLOAT;      break; }
			if (!strcmp(token_buf, "void")) {       tok.type = VOID;       break; }

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
	parse_error(&tok, "Token length is too long, max token length is %d\n", MAX_TOKEN_LEN);
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
		case RETURN:           puts("RETURN");         break;
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


token_value* peek_token(parsing_state* p, long offset)
{
	return GET_TOKEN_VAL(&p->tokens, p->pos+offset);
}

token_value* get_token(parsing_state* p)
{
	token_value* tok = GET_TOKEN_VAL(&p->tokens, p->pos++);
//	print_token(tok);
	//return GET_TOKEN_VAL(&p->tokens, p->pos++);
	return tok;
}

void parse_seek(parsing_state* p, int origin, long offset)
{
	if (origin == SEEK_SET)
		p->pos = offset;
	else if (origin = SEEK_CUR)
		p->pos += offset;
	else
		p->pos = p->tokens.size - 1 + offset;
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
	push_void(&p.tokens, &tok); //push END
	fclose(file);

	p.pos = 0;

	prog->cur_scope = 0;
	prog->func = NULL;
	prog->pc = NULL;
	prog->stmt_list = NULL;
	vec_void(&prog->functions, 0, 10, sizeof(function), free_function, init_function);  //I could actually use an init function here ..
	vec_str(&prog->global_variables, 0, 20);
	vec_void(&prog->global_values, 0, 20, sizeof(var_value), free_var_value, NULL);
	vec_void(&prog->expressions, 0, 300, sizeof(expression), free_expression, NULL);

	//
	// put this in a loop?  when body changes to function
//	tld - -> declration | function_definition

	translation_unit(&p, prog);

	free_vec_void(&p.tokens);
}


//	translation_unit -> top_level_declaration { top_level_declaration}
//
void translation_unit(parsing_state* p, program_state* prog)
{
	token_value* tok = peek_token(p, 0);
	while (tok->type != END) {
		top_level_declaration(p, prog);
		tok = peek_token(p, 0);
	}
}

void top_level_declaration(parsing_state* p, program_state* prog)
{
	var_type vtype = declaration_specifier(p, prog, 0);
	//will have to change peek(1 to account for more complex (longer than
	//one token) decl_specifiers in future
	if (vtype != UNKNOWN && peek_token(p, 1)->type == ID) {
		if (peek_token(p, 2)->type != LPAREN)
			declaration(p, prog);
		else
			function_definition(p, prog);
	} else {
		//TODO
		parse_error(peek_token(p, 0), "Error parsing top_level_declaration, declaration_specifier expected\n");
		exit(0);
	}
}



void function_definition(parsing_state* p, program_state* prog)
{
	var_type vtype = declaration_specifier(p, prog, 1);

	function_declarator(p, prog, vtype);

	compound_statement(p, prog);

	//functions are only global scope (and default extern)
	prog->func = NULL;
	prog->stmt_list = NULL;

	//since you can't have nested function defs, this will still be the current function
	function* func_ptr = back_void(&prog->functions);
	//erase all but parameters (first ones in the array)
	if (func_ptr->variables.size)
		erase_void(&func_ptr->variables, func_ptr->n_params, func_ptr->variables.size-1);
}

void function_declarator(parsing_state* p, program_state* prog, var_type vtype)
{
	//token should always be ID checked in top_level_decl
	token_value* tok = get_token(p);

	function a_func, *func_ptr;
	push_void(&prog->functions, &a_func);  //initialization is done automatically init_function
	assert(prog->functions.size <= 10);  //for now prevent possibility of bug

	var_value var;
	var.type = FUNCTION_TYPE;
	var.v.func_loc = prog->functions.size - 1;

	push_str(&prog->global_variables, tok->v.id);
	push_void(&prog->global_values, &var);

	func_ptr = back_void(&prog->functions);

	func_ptr->n_params = 0;
	func_ptr->ret_val.type = vtype;

	prog->func = func_ptr;
	prog->stmt_list = &func_ptr->stmt_list;

	tok = get_token(p);
	if (tok->type != LPAREN) {
		parse_error(tok, "in function_declarator, LPAREN expected\n");
		exit(0);
	}

	parameter_list(p, prog);

	tok = get_token(p);
	if (tok->type != RPAREN) {
		parse_error(tok, "in function_declarator, RPAREN expected\n");
		exit(0);
	}
}

void parameter_list(parsing_state* p, program_state* prog)
{
	token_value* tok = peek_token(p, 0);
	if (tok->type == VOID || tok->type == RPAREN) {
		prog->func->n_params = 0;
		if (tok->type == VOID)
			get_token(p);
		return;
	}
	
	parameter_declaration(p, prog);
	tok = peek_token(p, 0);
	while (tok->type == COMMA) {
		get_token(p);
		parameter_declaration(p, prog);
		tok = peek_token(p, 0);
	}
}

/* parameter_declaration -> decl_spec ID */
void parameter_declaration(parsing_state* p, program_state* prog)
{
	var_type vtype = declaration_specifier(p, prog, 1);

	token_value* tok = get_token(p);
	if (tok->type != ID) {
		parse_error(tok, "in parameter_declaration, ID expected\n");
		exit(0);
	}

	prog->func->n_params++;

	var_value* var = malloc(sizeof(var_value));;
	var->type = vtype;
	variable v;
	v.cur_scope = 0;
	v.name = mystrdup(tok->v.id);

	push_void(&prog->func->variables, &v);
	variable* tmp = back_void(&prog->func->variables);
	INIT_LIST_HEAD(&tmp->head);
	list_add(&var->list, &tmp->head);
	tmp->cur_scope++;
}


/* declaration -> declaration_specifier initialized_declarator_list ';' */
void declaration(parsing_state* p, program_state* prog)
{
	var_type vtype = declaration_specifier(p, prog, 1);  //should never be UNKNOWN because already validated

	initialized_declarator_list(p, prog, vtype);

	token_value* tok = get_token(p);
	if (tok->type != SEMICOLON) {
		parse_error(tok, "in declaration, COMMA or SEMICOLON expected.");
		exit(0);
	}
}

var_type declaration_specifier(parsing_state* p, program_state* prog, int match)
{
	token_value* tok = peek_token(p, 0);
	var_type type = UNKNOWN;

	switch (tok->type) {
	case INT:
		type = INT_TYPE;
		break;
	case DOUBLE:
		type = DOUBLE_TYPE;
		break;
	case VOID:
		type = VOID_TYPE;
		break;
	case UNKNOWN:
		if (match) {
			parse_error(tok, "in declaration_specifier, INT_TYPE expected\n");
			exit(0);
		}
	}

	if (match)
		get_token(p);
	
	return type;
}


/* initialized_declarator_list -> initialized_declarator { , initialized_declarator } */
void initialized_declarator_list(parsing_state* p, program_state* prog, var_type v_type)
{
	initialized_declarator(p, prog, v_type);

	token_value* tok = peek_token(p, 0);
	while (tok->type == COMMA) {
		get_token(p);

		initialized_declarator(p, prog, v_type);
		tok = peek_token(p, 0);
	}
}



/* initialized_declarator -> declarator [ = initializer ]
 *                        -> ID [ = assign_expr ]
 */
void initialized_declarator(parsing_state* p, program_state* prog, var_type v_type)
{
	token_value* tok = peek_token(p, 0);
	if (tok->type != ID) {
		parse_error(tok, "in initialized_declarator, ID expected.\n");
		exit(0);
	}


	//for functions push just for parsing/checking purposes like above
	//they'll be cleared (except parameters) at end of func_declaration
	//
	//no statements are made for global variables and they can only
	//have constant expressions
	if (prog->func) { 
		variable* check = look_up_variable(prog, tok->v.id);
		//TODO this does not yet handle hiding except function_level from global
		if (check && check->cur_scope == prog->cur_scope) {
			//TODO implement overload classes check here for whether it's actually ilegal see page 78
			parse_error(NULL, "in initialized_declarator, redeclaration of %s\n", tok->v.id);
			exit(0);
		}

		var_value* val = malloc(sizeof(var_value));
		val->type = v_type;

		if (!check) {
			variable v;
			v.name = mystrdup(tok->v.id);
			v.cur_scope = 0;
			push_void(&prog->func->variables, &v);
		}
		variable* tmp = (check) ? check : back_void(&prog->func->variables);
		if (!check)
			INIT_LIST_HEAD(&tmp->head);

		list_add(&val->list, &tmp->head);
		tmp->cur_scope++;

		statement decl_stmt;
		memset(&decl_stmt, 0, sizeof(statement));
		decl_stmt.type = DECL_STMT;
		decl_stmt.vtype = v_type;
		decl_stmt.lvalue = mystrdup(tok->v.id);

		push_void(prog->stmt_list, &decl_stmt);
		
		if (peek_token(p, 1)->type == EQUAL) {
			statement an_expr;
			memset(&an_expr, 0, sizeof(statement));
			an_expr.type = EXPR_STMT;

			an_expr.exp = make_expression(prog);
			assign_expr(p, prog, an_expr.exp);

			push_void(prog->stmt_list, &an_expr);
		} else {
			get_token(p);   //read ID
		}
	} else {
		if (look_up_value(prog, tok->v.id, ONLY_GLOBAL)) {
			//TODO implement overload classes check here for whether it's actually ilegal see page 78
			parse_error(NULL, "in initialized_declarator, redeclaration of %s\n", tok->v.id);
			exit(0);
		}

		var_value val;
		memset(&val, 0, sizeof(var_value)); //initialize globals
		val.type = v_type;

		push_str(&prog->global_variables, tok->v.id);
		push_void(&prog->global_values, &val);

		if (peek_token(p, 1)->type == EQUAL) {
			unsigned int e = make_expression(prog);
			assign_expr(p, prog, e);
			execute_expr(prog, e); //does the assignment inside
		} else {
			get_token(p); //read ID
		}
		
	}
	
}

void compound_statement(parsing_state* p, program_state* prog)
{
	token_value* tok = get_token(p); //should always be LBRACE

	vector_str scope_vars;
	vec_str(&scope_vars, 0, 20);

	prog->cur_scope++;

	declaration_or_statement_list(p, prog);

	pop_scope(prog, &scope_vars);
	free_vec_str(&scope_vars);

	prog->cur_scope--;

	tok = get_token(p);
	if (tok->type != RBRACE) {
		parse_error(tok, "in compound_statement, RBRACE expected.");
		exit(0);
	}
}


//TODO merge with decl_or_statement below?
void declaration_or_statement_list(parsing_state* p, program_state* prog)
{
	token_value* tok = peek_token(p, 0);
	while (tok->type != RBRACE) {
		declaration_or_statement(p, prog);
		tok = peek_token(p, 0);
	}
}

void declaration_or_statement(parsing_state* p, program_state* prog)
{
	var_type vtype = declaration_specifier(p, prog, 0);
	if (vtype != UNKNOWN) {
		declaration(p, prog);
	} else {
		statement_rule(p, prog);
	}
}


void statement_rule(parsing_state* p, program_state* prog)
{
	token_value* tok;

	switch (peek_token(p, 0)->type) {
	case ID:
		//could be label
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

	case LBRACE:
		{
		statement start, end;
		memset(&start, 0, sizeof(statement));
		memset(&end, 0, sizeof(statement));
		start.type = START_COMPOUND_STMT;
		end.type = END_COMPOUND_STMT;

		push_void(prog->stmt_list, &start);
		compound_statement(p, prog);
		push_void(prog->stmt_list, &end);
		}
		break;

	case GOTO:
		goto_stmt(p, prog);
		break;

	case RETURN:
		return_stmt(p, prog);
		break;

	default:
		parse_error(tok, "in statement unexpected token\n");
		exit(0);
	}
}

void goto_stmt(parsing_state* p, program_state* prog)
{
	token_value* tok;

	tok = get_token(p);

	//label? number?
}

void return_stmt(parsing_state* p, program_state* prog)
{
	get_token(p); //eat 'return'
	statement ret_stmt;
	memset(&ret_stmt, 0, sizeof(statement));

	ret_stmt.type = RETURN_STMT;

	if (peek_token(p, 0)->type != SEMICOLON) {
		ret_stmt.exp = make_expression(prog);
		expr(p, prog, ret_stmt.exp);
	} 

	push_void(prog->stmt_list, &ret_stmt);

	get_token(p); //eat ;
}

void expression_stmt(parsing_state* p, program_state* prog)
{
	statement an_expr;
	memset(&an_expr, 0, sizeof(statement));

	an_expr.exp = make_expression(prog);
	an_expr.type = EXPR_STMT;

	expr(p, prog, an_expr.exp);

	push_void(prog->stmt_list, &an_expr);


	token_value* tok = get_token(p);
	if (tok->type != SEMICOLON) {
		parse_error(tok, "in expression_stmt, SEMICOLON expected\n");
		exit(0);
	}
}

void expr(parsing_state* p, program_state* prog, unsigned int expr_loc)
{
	comma_expr(p, prog, expr_loc);
}

void comma_expr(parsing_state* p, program_state* prog, unsigned int expr_loc)
{
	assign_expr(p, prog, expr_loc);
	
	expression* e = GET_EXPRESSION(&prog->expressions, expr_loc);
	token_value* tok = peek_token(p, 0);
	while (tok->type == COMMA) {
		get_token(p);

		e->left = copy_expr(prog, expr_loc);
		e->tok.type = tok->type;

		e->right = make_expression(prog);
		
		assign_expr(p, prog, e->right);
		tok = peek_token(p, 0);
	}
}


int assignment_operator(Token tok)
{
	return (tok == EQUAL || tok == ADDEQUAL || tok == SUBEQUAL ||
	        tok == MULTEQUAL || tok == DIVEQUAL || tok == MODEQUAL);
}

/* assign_expr -> cond_expr | unary_expr assign_op assign_expr */
void assign_expr(parsing_state* p, program_state* prog, unsigned int expr_loc)
{
	token_value* tok;
	expression* e;	

	if (peek_token(p, 0)->type != ID) {
		cond_expr(p, prog, expr_loc);

		e = GET_EXPRESSION(&prog->expressions, expr_loc);
		if (assignment_operator(peek_token(p, 0)->type)) {
			if (e->tok.type != ID) {  //TODO for now
				parse_error(&e->tok, "in assign_expr, lvalue required as left operand of assignment.\n");	
				exit(0);
			}
		}
		return;
	}

	if (!assignment_operator(peek_token(p, 1)->type)) {
		cond_expr(p, prog, expr_loc);

		e = GET_EXPRESSION(&prog->expressions, expr_loc);
		if (assignment_operator(peek_token(p, 0)->type)) {
			if (e->tok.type != ID) {  //TODO for now
				parse_error(&e->tok, "in assign_expr, lvalue required as left operand of assignment.\n");	
				exit(0);
			}
		}
		return;
	}

	e = GET_EXPRESSION(&prog->expressions, expr_loc);
	e->tok.type = ID;
	e->tok.v.id = mystrdup(get_token(p)->v.id);
	
	tok = get_token(p); //get assignment op
	
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
	e->left = copy_expr(prog, expr_loc);
	e->tok.type = tok->type;

	e->right = make_expression(prog);

	assign_expr(p, prog, e->right);
}

unsigned int copy_expr(program_state* prog, unsigned int expr_loc)
{
	unsigned int copy = make_expression(prog);
	memcpy(GET_EXPRESSION(&prog->expressions, copy), GET_EXPRESSION(&prog->expressions, expr_loc), sizeof(expression));
	return copy;
}
	
void cond_expr(parsing_state* p, program_state* prog, unsigned int expr_loc)
{
	logical_or_expr(p, prog, expr_loc);

	//TODO add ternary operator here
}

/* logical_or_expr -> logical_and_expr { '||' logical_and_expr } */
void logical_or_expr(parsing_state* p, program_state* prog, unsigned int expr_loc)
{
	logical_and_expr(p, prog, expr_loc);

	expression* e = GET_EXPRESSION(&prog->expressions, expr_loc);
	while (peek_token(p, 0)->type == LOGICAL_OR) {
		get_token(p);

		e->left = copy_expr(prog, expr_loc);
		e->tok.type = LOGICAL_OR;

		e->right = make_expression(prog);

		logical_and_expr(p, prog, e->right);
	}
}


/* logical_and_expr -> bitwise_or_expr
 *                     logical_and_expr '&&' bitwise_or_expr
 * logical_and_expr -> equality_expr { '&&' equality_expr }
 */
void logical_and_expr(parsing_state* p, program_state* prog, unsigned int expr_loc)
{
	equality_expr(p, prog, expr_loc);
	expression* e = GET_EXPRESSION(&prog->expressions, expr_loc);

	while (peek_token(p, 0)->type == LOGICAL_AND) {
		get_token(p);

		e->left = copy_expr(prog, expr_loc);
		e->tok.type = LOGICAL_AND;

		e->right = make_expression(prog);
		
		equality_expr(p, prog, e->right);
	}
}

void equality_expr(parsing_state* p, program_state* prog, unsigned int expr_loc)
{
	relational_expr(p, prog, expr_loc);

	expression* e = GET_EXPRESSION(&prog->expressions, expr_loc);
	token_value* tok = peek_token(p, 0);
	while (tok->type == EQUALEQUAL || tok->type == NOTEQUAL) {
		get_token(p);

		e->left = copy_expr(prog, expr_loc);
		e->tok.type = tok->type;

		e->right = make_expression(prog);
		
		relational_expr(p, prog, e->right);
		tok = peek_token(p, 0);
	}
}


void relational_expr(parsing_state* p, program_state* prog, unsigned int expr_loc)
{
	//skiping shift operators would be shift_expr() here
	//
	add_expr(p, prog, expr_loc);
	expression* e = GET_EXPRESSION(&prog->expressions, expr_loc);

	token_value* tok = peek_token(p, 0);
	while (tok->type == GREATER || tok->type == GTEQ || tok->type == LESS || tok->type == LTEQ) {
		get_token(p);
		e->left = copy_expr(prog, expr_loc);
		e->tok.type = tok->type;

		e->right = make_expression(prog);
		add_expr(p, prog, e->right);
		
		tok = peek_token(p, 0);
	}
}


/* add_expr -> mult_expr { '+'|'-' mult_expr } */
void add_expr(parsing_state* p, program_state* prog, unsigned int expr_loc)
{
	mult_expr(p, prog, expr_loc);
	expression* e = GET_EXPRESSION(&prog->expressions, expr_loc);

	token_value* tok = peek_token(p, 0);
	while (tok->type == ADD || tok->type == SUB) {
		get_token(p);
		
		e->left = copy_expr(prog, expr_loc);
		e->tok.type = tok->type;

		e->right = make_expression(prog);
		mult_expr(p, prog, e->right);

		tok = peek_token(p, 0);
	}
}

/* mult_expr ->  unary_expr { '*'|'/' unary_expr } */
void mult_expr(parsing_state* p, program_state* prog, unsigned int expr_loc)
{
	token_value* tok;

	unary_expr(p, prog, expr_loc);
	expression* e = GET_EXPRESSION(&prog->expressions, expr_loc);

	tok = peek_token(p, 0);
	while (tok->type == MULT || tok->type == DIV || tok->type == MOD) {
		get_token(p);

		e->left = copy_expr(prog, expr_loc);
		e->tok.type = tok->type;

		e->right = make_expression(prog);
		unary_expr(p, prog, e->right);

		tok = peek_token(p, 0);
	}
}

void unary_expr(parsing_state* p, program_state* prog, unsigned int expr_loc)
{
	postfix_expr(p, prog, expr_loc);
}


void postfix_expr(parsing_state* p, program_state* prog, unsigned int expr_loc)
{
	if (peek_token(p, 0)->type == ID && peek_token(p, 1)->type == LPAREN) {
		function_call(p, prog, expr_loc);
	} else {
		primary_expr(p, prog, expr_loc);
	}
}


void function_call(parsing_state* p, program_state* prog, unsigned int expr_loc)
{
	token_value* tok = get_token(p);
	get_token(p);  //get '('
	
	expression* e = GET_EXPRESSION(&prog->expressions, expr_loc);
	e->tok.type = FUNC_CALL;

	e->left = make_expression(prog);

	expression* left = GET_EXPRESSION(&prog->expressions, e->left);
	left->tok.type = ID;
	left->tok.v.id = mystrdup(tok->v.id);

	//this check will have to change when I allow function pointers
	var_value* check = look_up_value(prog, tok->v.id, ONLY_GLOBAL);
	if (!check) {
		parse_error(tok, "implicit declaration of function '%s'.\n", tok->v.id);
		exit(0);
	} else if (check->type != FUNCTION_TYPE) {
		parse_error(tok, "called object '%s' is not a function\n", tok->v.id); 
		exit(0);
	}


	e->right = make_expression(prog);
	
	if (peek_token(p, 0)->type == RPAREN) {
		expression* right = GET_EXPRESSION(&prog->expressions, e->right);
		right->tok.type = VOID;
		get_token(p);
		return;
	}
	expression_list(p, prog, e->right);

	tok = get_token(p);
	if (tok->type != RPAREN) {
		parse_error(tok, "in function_call, ')' expected\n");
		exit(0);
	}
}

void expression_list(parsing_state* p, program_state* prog, unsigned int expr_loc)
{
	assign_expr(p, prog, expr_loc);
	expression* e = GET_EXPRESSION(&prog->expressions, expr_loc);

	if (peek_token(p, 0)->type == COMMA) {
		get_token(p);

		e->left = copy_expr(prog, expr_loc);
		e->tok.type = EXPR_LIST;

		e->right = make_expression(prog);

		expression_list(p, prog, e->right);
	}
}

/* primary -> '(' expr ')' | ID | *_LITERAL */
void primary_expr(parsing_state* p, program_state* prog, unsigned int expr_loc)
{
	token_value* tok = get_token(p);
	expression* e = GET_EXPRESSION(&prog->expressions, expr_loc);

	switch (tok->type) {
	case LPAREN:
		e->tok.type = EXP;
		e->left = make_expression(prog);
		assert(e->left);
		expr(p, prog, e->left);

		tok = get_token(p);
		if (tok->type != RPAREN) {
			parse_error(tok, "in primary_expr, RPAREN expected.\n");
			exit(0);
		}
		break;
	case ID:
		if (!prog->func) {
			parse_error(tok, "global variable initializer element is not a constant\n");
			exit(0);
		}
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
		parse_error(tok, "in primary_expr, LPAREN or literal expected\n");
		if (tok->type == ADDEQUAL)
			puts("plusequal");
		exit(0);
	}
}



void if_stmt(parsing_state* p, program_state* prog)
{
	get_token(p); //get if

	statement an_if;
	memset(&an_if, 0, sizeof(statement));
	an_if.type = IF_STMT;

	an_if.exp = make_expression(prog);

	cond_expr(p, prog, an_if.exp);

	size_t if_loc = prog->stmt_list->size;
	push_void(prog->stmt_list, &an_if);

	statement_rule(p, prog);

	GET_STMT(prog->stmt_list, if_loc)->jump_to = prog->stmt_list->size;
}


void print_stmt(parsing_state* p, program_state* prog)
{
	get_token(p); //get print

	token_value* tok = get_token(p);

	if (tok->type != ID) {
		parse_error(tok, "in print_stmt, ID expected.\n");
		exit(0);
	}

	statement a_print;
	memset(&a_print, 0, sizeof(statement));
	a_print.type = PRINT_STMT;

	a_print.lvalue = mystrdup(tok->v.id);
	
	tok = get_token(p);
	if (tok->type != SEMICOLON) {
		parse_error(tok, "in print_stmt, SEMICOLON expected.\n");
		exit(0);
	}

	push_void(prog->stmt_list, &a_print);
}



void while_stmt(parsing_state* p, program_state* prog)
{
	get_token(p); //get while

	statement a_while;
	memset(&a_while, 0, sizeof(statement));
	a_while.type = WHILE_STMT;

	a_while.exp = make_expression(prog);

	cond_expr(p, prog, a_while.exp);

	size_t while_loc = prog->stmt_list->size;
	push_void(prog->stmt_list, &a_while);

	statement_rule(p, prog);

	statement a_goto;
	memset(&a_goto, 0, sizeof(statement));
	a_goto.type = GOTO_STMT;
	a_goto.jump_to = while_loc;
	push_void(prog->stmt_list, &a_goto);

	GET_STMT(prog->stmt_list, while_loc)->jump_to = prog->stmt_list->size;
}







void parse_error(token_value* tok, char *str, ...)
{
	va_list args;
	va_start(args, str);
	fprintf(stderr, "Parse Error: ");
	vfprintf(stderr, str, args);
	if (tok) {
		fprintf(stderr, "Got ");
		print_token(tok);
	}
	va_end(args);
}
