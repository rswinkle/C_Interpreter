#include "parser.h"

#include "c_utils.h"

#define CVECTOR_IMPLEMENTATION
#include "cvector.h"

#define CVECTOR_token_lex_IMPLEMENTATION
#include "cvector_token_lex.h"

#include <stdio.h>

CVEC_NEW_DEFS2(var_value, RESIZE)

CVEC_NEW_DEFS2(expr_block, RESIZE)

CVEC_NEW_DEFS2(function, RESIZE)


// See comment in parser.h explaining the reason for expr_block
expression* make_expression(program_state* prog)
{
	expr_block* b = cvec_back_expr_block(&prog->expressions);
	if (b->n == b->used) {
		expr_block b_new;
		make_expression_block(32, &b_new);
		cvec_push_expr_block(&prog->expressions, &b_new);
		b = cvec_back_expr_block(&prog->expressions);
	}
	return &b->data[b->used++];
}



int make_expression_block(size_t n, expr_block* block)
{
	block->data = malloc(n * sizeof(expression)); assert(block->data);
	if (!block->data) {
		return 0;
	}
	block->n = n;
	block->used = 0;
	return 1;
}

void free_expr_block(void* block)
{
	expr_block* b = block;
	free(b->data);
}

void free_active_binding_list(void* l)
{
	list_head* head = l;
	active_binding* p, *tmp;

	list_for_each_entry_safe(p, tmp, active_binding, head, list) {
		list_del(&p->list);
		free(p);
	}
}

void free_symbol(void* var)
{
	symbol* s = var;
	free_active_binding_list(&s->head);
}

// Used as a regular constructor, we don't use from
void init_function(void* to, void* from)
{
	function* to_func = to;

	to_func->pc = 0;
	cvec_void(&to_func->stmt_list, 0, 50, sizeof(statement), free_statement, NULL);
	cvec_void(&to_func->symbols, 0, 20, sizeof(symbol), free_symbol, NULL);
}

void free_function(void* func)
{
	function* f = func;
	cvec_free_void(&f->stmt_list);
	cvec_free_void(&f->symbols);
}


void free_statement(void* stmt)
{
	statement* s = (statement*)stmt;
	cvec_free_void_heap(s->bindings);
}


void free_var_value(void* var)
{
	//var_value *v = var;
	// TODO when necessary
	// nothing to free for now
}


void print_statement(statement* stmt)
{
	switch (stmt->type) {
	case NULL_STMT:                  puts("NULL_STMT");  break;
	case WHILE_STMT:              puts("WHILE_STMT");  break;
	case PRINT_STMT:               puts("PRINT_STMT");  break;
	case EXPR_STMT:                  puts("EXPR_STMT");  break;
	case IF_STMT:                  puts("IF_STMT");  break;
	case GOTO_STMT:                  puts("GOTO_STMT");  break;
	case FOR_STMT:                  puts("FOR_STMT");  break;
	case CONTINUE_STMT:              puts("CONTINUE_STMT");  break;
	case BREAK_STMT:              puts("BREAK_STMT");  break;
	case DO_STMT:     		         puts("DO_STMT");  break;
	case CASE_STMT:              puts("CASE_STMT");  break;
	case DEFAULT_STMT:              puts("DEFAULT_STMT");  break;
	case SWITCH_STMT:              puts("SWITCH_STMT");  break;
	case DECL_STMT:                  puts("DECL_STMT");  break;
	case RETURN_STMT:              puts("RETURN_STMT");  break;
	case START_COMPOUND_STMT:      puts("START_COMPOUND_STMT");  break;
	case END_COMPOUND_STMT:                puts("END_COMPOUND_STMT");  break;

	default:
		puts("Unknown statement type");
	}
}


void print_type(var_value* v)
{
	switch (v->type) {
	case CHAR_TYPE:      puts("CHAR_TYPE"); break;
	case UCHAR_TYPE:     puts("UCHAR_TYPE"); break;
	case SHORT_TYPE:     puts("SHORT_TYPE"); break;
	case USHORT_TYPE:    puts("USHORT_TYPE"); break;
	case INT_TYPE:       puts("INT_TYPE"); break;
	case UINT_TYPE:      puts("UINT_TYPE"); break;
	case LONG_TYPE:      puts("LONG_TYPE"); break;
	case ULONG_TYPE:     puts("ULONG_TYPE"); break;
	case FLOAT_TYPE:     puts("FLOAT_TYPE"); break;
	case DOUBLE_TYPE:    puts("DOUBLE_TYPE"); break;
	case FUNCTION_TYPE:  puts("FUNCTION_TYPE"); break;
	case VOID_TYPE:      puts("VOID_TYPE"); break;
	case INT_PTR_TYPE:   puts("INT_PTR_TYPE"); break;
	default:
		puts("unknown type in print_type");
	}
}

token_value* peek_token(parsing_state* p, long offset)
{
	return &p->tokens.a[p->pos + offset].tok;
}

token_value* get_token(parsing_state* p)
{
	token_value* tok = &p->tokens.a[p->pos++].tok;
	//print_token(tok, stdout, 0); putchar('\n');

	//return &p->tokens.a[p->pos++].tok;
	return tok;
}

void parse_seek(parsing_state* p, int origin, long offset)
{
	if (origin == SEEK_SET)
		p->pos = offset;
	else if (origin == SEEK_CUR)
		p->pos += offset;
	else
		p->pos = p->tokens.size - 1 + offset;
}

void parse_program_string(program_state* prog, char* string)
{
	parsing_state p;
	cvec_token_lex(&p.tokens, 0, 1000, NULL, NULL);

	cvec_str(&prog->string_db, 0, 100);
	int i;

	//starts on line 1
	lexer_state lexer = { NULL, 1, 0, 0, 0 };

	token_lex tok_lex;
	tok_lex = read_token_from_str(string, &lexer, PARSING);
	while (tok_lex.tok.type != END) {
		if (tok_lex.tok.type == ID || tok_lex.tok.type == STR_LITERAL) {
			for (i=0; i<prog->string_db.size; ++i) {
				if (!strcmp(tok_lex.tok.v.id, prog->string_db.a[i])) {
					free(tok_lex.tok.v.id);
					tok_lex.tok.v.id = prog->string_db.a[i];
					break;
				}
			}
			if (i == prog->string_db.size) {
				cvec_extend_str(&prog->string_db, 1);
				prog->string_db.a[prog->string_db.size-1] = tok_lex.tok.v.id;
			}
		}
		cvec_push_token_lex(&p.tokens, &tok_lex);
		tok_lex = read_token_from_str(string, &lexer, PARSING);
	}
	cvec_push_token_lex(&p.tokens, &tok_lex); //push END
	free(lexer.cur_file);

	p.pos = 0;

	prog->cur_parent = -1;
	prog->cur_iter = 0;
	prog->cur_iter_switch = 0;
	prog->func = NULL;
	prog->pc = NULL;
	prog->stmt_list = NULL;
	prog->bindings = NULL;
	cvec_function(&prog->functions, 0, 10, free_function, init_function);
	cvec_str(&prog->global_variables, 0, 20);
	cvec_var_value(&prog->global_values, 0, 20, free_var_value, NULL);

	cvec_expr_block(&prog->expressions, 1, 1, free_expr_block, NULL);
	make_expression_block(50, cvec_back_expr_block(&prog->expressions));

	translation_unit(&p, prog);

	cvec_free_token_lex(&p.tokens);
}

void parse_program_file(program_state* prog, FILE* file)
{
	parsing_state p;
	cvec_token_lex(&p.tokens, 0, 1000, NULL, NULL);

	cvec_str(&prog->string_db, 0, 100);
	int i;

	//starts on line 1
	lexer_state lexer = { NULL, 1, 0, 0, 0 };

	token_lex tok_lex = read_token(file, &lexer, PARSING);
	while (tok_lex.tok.type != END) {
		if (tok_lex.tok.type == ID || tok_lex.tok.type == STR_LITERAL) {
			for (i=0; i<prog->string_db.size; ++i) {
				if (!strcmp(tok_lex.tok.v.id, prog->string_db.a[i])) {
					free(tok_lex.tok.v.id);
					tok_lex.tok.v.id = prog->string_db.a[i];
					break;
				}
			}
			if (i == prog->string_db.size) {
				cvec_extend_str(&prog->string_db, 1);
				prog->string_db.a[prog->string_db.size-1] = tok_lex.tok.v.id;
			}
		}

		// debugging
		//print_token(&tok_lex.tok, stdout, 0);
		//putchar('\n');

		cvec_push_token_lex(&p.tokens, &tok_lex);
		tok_lex = read_token(file, &lexer, PARSING);
	}
	cvec_push_token_lex(&p.tokens, &tok_lex); //push END
	free(lexer.cur_file);
	fclose(file);


	p.pos = 0;

	prog->cur_parent = -1;
	prog->cur_iter = 0;
	prog->cur_iter_switch = 0;
	prog->func = NULL;
	prog->pc = NULL;
	prog->stmt_list = NULL;
	prog->bindings = NULL;
	cvec_function(&prog->functions, 0, 10, free_function, init_function);
	cvec_str(&prog->global_variables, 0, 20);
	cvec_var_value(&prog->global_values, 0, 20, free_var_value, NULL);

	cvec_expr_block(&prog->expressions, 1, 1, free_expr_block, NULL);
	make_expression_block(50, cvec_back_expr_block(&prog->expressions));

	translation_unit(&p, prog);

	cvec_free_token_lex(&p.tokens);
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
	int n_tokens;
	var_type vtype = declaration_specifier(p, prog, 0, &n_tokens);

	if (vtype != UNKNOWN && peek_token(p, n_tokens)->type == ID) {
		if (peek_token(p, n_tokens+1)->type != LPAREN)
			declaration(p, prog);
		else
			function_definition(p, prog);
	} else {
		parse_error(peek_token(p, 0), "Error parsing top_level_declaration, declaration_specifier expected");
	}
}


/* function_definition -> function_declarator ';'  <- function declaration/prototype
 *                        function_declarator compound_statement
 */
void function_definition(parsing_state* p, program_state* prog)
{
	var_type vtype = declaration_specifier(p, prog, 1, NULL);

	function_declarator(p, prog, vtype);

	token_value* tok = peek_token(p, 0);
	if (tok->type == SEMICOLON) {
		get_token(p);
		return;
	} else if (tok->type != LBRACE) {
		parse_error(tok, "expected ';' or '{' for function declaration or definition, ");
	}

	cvec_str(&prog->func->labels, 0, 10);
	cvec_i(&prog->func->label_locs, 0, 10);

	compound_statement(p, prog);

	//set goto locations
	statement* stmt;
	for (int j=0; j<prog->func->labels.size; ++j) {
		for (int i=0; i<prog->stmt_list->size; ++i) {
			stmt = GET_STMT(prog->stmt_list, i);
			if (stmt->type == GOTO_STMT &&
			    !strcmp(stmt->lvalue, prog->func->labels.a[j])) {

				stmt->jump_to = prog->func->label_locs.a[j];
			}
		}
	}

	cvec_free_str(&prog->func->labels);
	cvec_free_i(&prog->func->label_locs);

	//functions are only global scope (and default extern)
	prog->func = NULL;
	prog->stmt_list = NULL;
}

void function_declarator(parsing_state* p, program_state* prog, var_type vtype)
{
	//token should always be ID checked in top_level_decl
	token_value* tok = get_token(p);

	//TODO make sure it's not already defined and make sure it matches previous declarations

	function* func_ptr;
	cvec_push_function(&prog->functions, NULL);  //initialization is done automatically in init_function
	assert(prog->functions.size <= 10);  //for now prevent possibility of bug TODO what was this for?

	var_value var;
	var.type = FUNCTION_TYPE;
	var.v.func_loc = prog->functions.size - 1;

	cvec_push_str(&prog->global_variables, tok->v.id);
	cvec_push_var_value(&prog->global_values, &var);

	func_ptr = cvec_back_function(&prog->functions);

	func_ptr->n_params = 0;
	func_ptr->ret_val.type = vtype;

	prog->func = func_ptr;
	prog->stmt_list = &func_ptr->stmt_list;

	tok = get_token(p);
	if (tok->type != LPAREN) {
		parse_error(tok, "in function_declarator, LPAREN expected\n");
	}

	parameter_list(p, prog);

	tok = get_token(p);
	if (tok->type != RPAREN) {
		parse_error(tok, "in function_declarator, RPAREN expected\n");
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
	var_type vtype = declaration_specifier(p, prog, 1, NULL);

	token_value* tok = get_token(p);
	if (tok->type != ID) {
		parse_error(tok, "in parameter_declaration, ID expected\n");
	}

	var_value* v = look_up_value(prog, tok->v.id, ONLY_LOCAL);
	if (v) {
		parse_error(tok, "in parameter_declaration, redeclaration of symbol\n");
	}

	prog->func->n_params++;

	//parameters aren't freed from the symbol table till exit
	//they need storage to assign to from the caller's scope in
	//execute_expr_list
	active_binding* var = malloc(sizeof(active_binding));
	if (!var) { //TODO not really parse error ... but it did occur during parsing so?
		parse_error(tok, "in parameter_declaration, memory allocation failure\n");
	}
	var->val.type = vtype;
	symbol s;
	s.cur_parent = 0;
	s.name = tok->v.id;

	cvec_push_void(&prog->func->symbols, &s);
	symbol* tmp = cvec_back_void(&prog->func->symbols);
	INIT_LIST_HEAD(&tmp->head);
	list_add(&var->list, &tmp->head);
}


/* declaration -> declaration_specifier initialized_declarator_list ';' */
void declaration(parsing_state* p, program_state* prog)
{
	var_type vtype = declaration_specifier(p, prog, 1, NULL);  //should never be UNKNOWN because already validated

	initialized_declarator_list(p, prog, vtype);

	token_value* tok = get_token(p);
	if (tok->type != SEMICOLON) {
		parse_error(tok, "in declaration, COMMA or SEMICOLON expected.");
	}
}


/* auto extern register static typedef
 * probably will only support static and typedef
void storage_specifier(parsing_state* p, program_state* prog, int match)
{
	token_value* tok = peek_token(p, 0);
	switch (tok->type) {
	case EXTERN:

		break;
	case STATIC:

		break;
	case TYPEDEF:

		break;

	default:
		if (tok->type == AUTO && prog->cur_parent == -1) {
			parse_error(tok, "auto storage specifer only allowed within a block\n");
		}
	}

}
*/

/* equivalent names (and only names accepted, no mixing the order):
 *
 * char, signed char
 * short, short int, signed short, signed short int
 * int, signed int, signed
 * long, long int, signed long, signed long int
 * long long, long long int, signed long long, signed long long int //TODO
 *
 * unsigned char
 * unsigned short, unsigned short int
 * unsigned, unsigned int
 * unsigned long, unsigned long int
 * unsigned long long, unsigned long long int //TODO
 *
 * float
 * double
 * long double //TODO
 */
var_type declaration_specifier(parsing_state* p, program_state* prog, int match, int* tokens)
{
	//my array cheat prevents me from using peek_token and token_value
	token_lex* tok = &p->tokens.a[p->pos];

	var_type type = UNKNOWN;

	int seek = 1;

	switch (tok->tok.type) {
	case CHAR:
		type = CHAR_TYPE;
		break;
	case SHORT:
		if (tok[1].tok.type == INT)
			seek++;
		type = SHORT_TYPE;
		break;
	case INT:
		type = INT_TYPE;
		break;
	case LONG:
		if (tok[1].tok.type == INT)
			seek++;
		type = LONG_TYPE;
		break;
	case UNSIGNED:
		switch (tok[1].tok.type) {
		case CHAR:
			type = UCHAR_TYPE;
			seek++;
			break;
		case SHORT:
			type = USHORT_TYPE;
			seek++;
			if (tok[2].tok.type == INT)
				seek++;
			break;
		case LONG:
			type = ULONG_TYPE;
			seek++;
			if (tok[2].tok.type == INT)
				seek++;
			break;
		case INT:
			seek++;
		default:
			type = UINT_TYPE;
			break;
		}
		break;

	case SIGNED:
		switch (tok[1].tok.type) {
		case CHAR:
			type = CHAR_TYPE; //assume plain char is signed
			seek++;
			break;
		case SHORT:
			type = SHORT_TYPE;
			seek++;
			if (tok[2].tok.type == INT)
				seek++;
			break;
		case LONG:
			type = LONG_TYPE;
			seek++;
			if (tok[2].tok.type == INT)
				seek++;
			break;
		case INT:
			seek++;
		default:
			type = INT_TYPE;
			break;
		}
		break;

	case FLOAT:
		type = FLOAT_TYPE;
		break;
	case DOUBLE:
		type = DOUBLE_TYPE;
		break;
	case VOID:
		type = VOID_TYPE;
		break;

	default:
		if (match) {
			parse_error(&tok->tok, "in declaration_specifier, valid type expected\n");
		}
	}

	if (match) {
		for (int i=0; i<seek; ++i)
			get_token(p); //makes it easier to debug otherwise I'd just seek
	}
	if (tokens)
		*tokens = seek;

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


	//pointer_declarator or direct_declarator
	if (tok->type != ID) {
		parse_error(tok, "in initialized_declarator, ID expected.\n");
	}


	//for functions push just for parsing/checking purposes like above
	//they'll be cleared (except parameters) at end of func_declaration
	//
	//no statements are made for global variables and they can only
	//have constant expressions
	if (prog->func) {
		symbol* check = look_up_symbol(prog, tok->v.id);
		//TODO this does not yet handle hiding except function_level from global
		if (check && check->cur_parent == prog->cur_parent) {
			//TODO implement overload classes check here for whether it's actually ilegal see page 78
			parse_error(NULL, "in initialized_declarator, redeclaration of %s\n", tok->v.id);
		}

		active_binding* val = malloc(sizeof(active_binding));
		if (!val) { //TODO not really parse error
			parse_error(tok, "in initialized_declarator, memory allocation failure\n");
		}
		val->val.type = v_type;
		val->parent = prog->cur_parent;

		if (!check) {
			symbol s;
			s.name = tok->v.id;
			cvec_push_void(&prog->func->symbols, &s);
		}
		symbol* tmp = (check) ? check : cvec_back_void(&prog->func->symbols);
		if (!check)
			INIT_LIST_HEAD(&tmp->head);

		list_add(&val->list, &tmp->head);
		tmp->cur_parent = prog->cur_parent;

		statement decl_stmt;
		memset(&decl_stmt, 0, sizeof(statement));
		decl_stmt.type = DECL_STMT;
		decl_stmt.vtype = v_type;
		decl_stmt.lvalue = tok->v.id;
		decl_stmt.parent = prog->cur_parent;

		binding b;
		b.name = tmp->name;
		b.vtype = v_type;
		b.decl_stmt = prog->stmt_list->size;
		cvec_push_void(prog->bindings, &b);

		cvec_push_void(prog->stmt_list, &decl_stmt);

		if (peek_token(p, 1)->type == EQUAL) {
			statement an_expr;
			memset(&an_expr, 0, sizeof(statement));
			an_expr.type = EXPR_STMT;

			an_expr.exp = make_expression(prog);
			assign_expr(p, prog, an_expr.exp);

			cvec_push_void(prog->stmt_list, &an_expr);
		} else {
			get_token(p);   //read ID
		}
	} else {
		if (look_up_value(prog, tok->v.id, ONLY_GLOBAL)) {
			//TODO implement overload classes check here for whether it's actually ilegal see page 78
			parse_error(NULL, "in initialized_declarator, redeclaration of %s\n", tok->v.id);
		}

		var_value val;
		memset(&val, 0, sizeof(var_value)); //initialize globals
		val.type = v_type;

		cvec_push_str(&prog->global_variables, tok->v.id);
		cvec_push_var_value(&prog->global_values, &val);

		if (peek_token(p, 1)->type == EQUAL) {
			expression* e = make_expression(prog);
			assign_expr(p, prog, e);
			execute_constant_expr(prog, e); //does the assignment inside
		} else {
			get_token(p); //read ID
		}

	}
}

void compound_statement(parsing_state* p, program_state* prog)
{
	token_value* tok = get_token(p); //should always be LBRACE


	statement start, end;
	memset(&start, 0, sizeof(statement));
	memset(&end, 0, sizeof(statement));
	start.type = START_COMPOUND_STMT;
	end.type = END_COMPOUND_STMT;

	//set parent to current scope which will be -1 if it's the start of a function definition
	//and it'll be the first statement (index 0).  if it's not then a containing compound statement
	//will change it in the loop below
	start.parent = prog->cur_parent;

	cvector_void* old_bindings = prog->bindings;
	int old_parent = prog->cur_parent;

	start.bindings = cvec_void_heap(0, 20, sizeof(binding), NULL, NULL);
	prog->bindings = start.bindings;

	end.parent = prog->stmt_list->size;
	cvec_push_void(prog->stmt_list, &start);

	prog->cur_parent = end.parent;

	declaration_or_statement_list(p, prog);

	cvec_push_void(prog->stmt_list, &end);

	prog->bindings = old_bindings;
	prog->cur_parent = old_parent;


	//TODO more efficient way to do this
	binding* b;
	for (int i=0; i<start.bindings->size; ++i) {
		b = GET_BINDING(start.bindings, i);
		remove_binding(prog, b->name);
	}

	tok = get_token(p);
	if (tok->type != RBRACE) {
		parse_error(tok, "in compound_statement, RBRACE expected.");
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
	var_type vtype = declaration_specifier(p, prog, 0, NULL);
	if (vtype != UNKNOWN) {
		declaration(p, prog);
	} else {
		statement_rule(p, prog);
	}
}


void statement_rule(parsing_state* p, program_state* prog)
{

	switch (peek_token(p, 0)->type) {
	case ID:
		//could be label
		if (peek_token(p, 1)->type != COLON)
			expression_stmt(p, prog);
		else
			labeled_stmt(p, prog);
		break;

	case FOR:
		for_stmt(p, prog);
		break;

	case DO:
		do_stmt(p, prog);
		break;

	case WHILE:
		while_stmt(p, prog);
		break;

	case SWITCH:
		switch_stmt(p, prog);
		break;

	case CASE:
	case DEFAULT:
		case_or_default_stmt(p, prog);
		break;

	case IF:
		if_stmt(p, prog);
		break;

	case PRINT:
		print_stmt(p, prog);
		break;

	case LBRACE:
		compound_statement(p, prog);
		break;

	case GOTO:
		goto_stmt(p, prog);
		break;

	case BREAK:
	case CONTINUE:
		break_or_continue_stmt(p, prog);
		break;

	case RETURN:
		return_stmt(p, prog);
		break;

	case SEMICOLON:
	{
		get_token(p); //eat semicolon
		statement null_stmt;
		memset(&null_stmt, 0, sizeof(statement));
		null_stmt.type = NULL_STMT;
		null_stmt.parent = prog->cur_parent;
		cvec_push_void(prog->stmt_list, &null_stmt);
	}
		break;

	default:
		expression_stmt(p, prog);
/*
		parse_error(peek_token(p, 0), "in statement unexpected token\n");
		*/
	}
}


int is_integral_type(var_value* v)
{
	switch (v->type) {
	case INT_TYPE:  //All INT_LITERALS are returned as INT_TYPE
		return 1;
	default:
		return 0; // suppress compiler unhandled enums warning
	}
}




void case_or_default_stmt(parsing_state* p, program_state* prog)
{
	token_value* tok = get_token(p);

	statement stmt;
	memset(&stmt, 0, sizeof(statement));
	stmt.type = (tok->type == CASE) ? CASE_STMT : DEFAULT_STMT;
	stmt.parent = prog->cur_parent;

	var_value v;
	if (stmt.type == CASE_STMT) {
		stmt.exp = make_expression(prog);
		cond_expr(p, prog, stmt.exp);
		v = execute_constant_expr(prog, stmt.exp);
		if (!is_integral_type(&v)) {
			parse_error(NULL, "case label does not reduce to integer constant\n");
		}
		//TODO
		stmt.case_val = v.v.int_val;
	}

	cvec_push_void(prog->stmt_list, &stmt);

	tok = get_token(p);
	if (tok->type != COLON) {
		parse_error(tok, "in case or default statement expected COLON\n");
	}
}

void switch_stmt(parsing_state* p, program_state* prog)
{
	get_token(p);
	token_value* tok = get_token(p);
	if (tok->type != LPAREN) {
		parse_error(tok, "in switch_stmt, expected LPAREN\n");
	}

	statement a_switch;
	memset(&a_switch, 0, sizeof(statement));
	a_switch.type = SWITCH_STMT;
	a_switch.parent = prog->cur_parent;
	a_switch.exp = make_expression(prog);

	expr(p, prog, a_switch.exp);
	//TODO when I add more types make sure expression is integral type

	tok = get_token(p);
	if (tok->type != RPAREN) {
		parse_error(tok, "in switch_stmt, expected RPAREN\n");
	}

	int old_i_switch = prog->cur_iter_switch;
	prog->cur_iter_switch = prog->stmt_list->size;

	cvec_push_void(prog->stmt_list, &a_switch);
	statement* the_switch = (statement*)cvec_back_void(prog->stmt_list);

	statement_rule(p, prog);

	//I know, I know another terrible violation of the one variable,
	//one use/meaning principle ...
	the_switch->bindings = cvec_void_heap(0, 10, sizeof(a_case), NULL, NULL);
	statement* stmt;
	a_case c;
	int j;
	for (int i=prog->cur_iter_switch; i<prog->stmt_list->size; ++i) {
		stmt = GET_STMT(prog->stmt_list, i);
		if (stmt->type == BREAK_STMT && stmt->jump_to == 0) {
			stmt->type = GOTO_STMT;
			stmt->jump_to =  prog->stmt_list->size;
		} else if (stmt->type == CASE_STMT) {
			c.val = stmt->case_val;
			/* probably not worth it since the interpreter can just fall through
 			 * case statements anyway but if I decide to erase case statements ..*/
			j = i + 1;
			while (GET_STMT(prog->stmt_list, j)->type == CASE_STMT)
				j++;

			c.jump_to = j;
			cvec_push_void(the_switch->bindings, &c);
		} else if (stmt->type == DEFAULT_STMT) {
			if (the_switch->jump_to) {
				parse_error(NULL, "more than one default statement in switch\n");
			}
			j = i + 1;
			while (GET_STMT(prog->stmt_list, j)->type == CASE_STMT)
				j++;
			the_switch->jump_to = j;
		}
	}
	//if no default then jump to end
	if (!the_switch->jump_to)
		the_switch->jump_to = prog->stmt_list->size;

	prog->cur_iter_switch = old_i_switch;
}

void do_stmt(parsing_state* p, program_state* prog)
{
	get_token(p);

	size_t start_loc = prog->stmt_list->size;
	int old_iter = prog->cur_iter;
	int old_i_switch = prog->cur_iter_switch;

	prog->cur_iter = prog->cur_iter_switch = start_loc;

	statement_rule(p, prog);

	prog->cur_iter = old_iter;
	prog->cur_iter_switch = old_i_switch;

	token_value* tok = get_token(p);
	if (tok->type != WHILE) {
		parse_error(tok, "in do_stmt expected WHILE\n");
	}
	tok = get_token(p);
	if (tok->type != LPAREN) {
		parse_error(tok, "in do_stmt epected LPAREN\n");
	}

	statement do_stmt;
	memset(&do_stmt, 0, sizeof(statement));
	do_stmt.type = DO_STMT;
	do_stmt.parent = prog->cur_parent;
	do_stmt.jump_to = start_loc;

	do_stmt.exp = make_expression(prog);

	expr(p, prog, do_stmt.exp);

	cvec_push_void(prog->stmt_list, &do_stmt);

	tok = get_token(p);
	if (tok->type != RPAREN) {
		parse_error(tok, "in do_stmt epected RPAREN\n");
	}

	statement* stmt;
	for (int i=start_loc; i<prog->stmt_list->size; ++i) {
		stmt = GET_STMT(prog->stmt_list, i);
		if (stmt->type == BREAK_STMT && stmt->jump_to == 0) {
			stmt->type = GOTO_STMT;
			stmt->jump_to =  prog->stmt_list->size;
		} else if (stmt->type == CONTINUE_STMT && stmt->jump_to == 0) {
			stmt->type = GOTO_STMT;
			stmt->jump_to = prog->stmt_list->size - 1;
		}
	}
}

void break_or_continue_stmt(parsing_state* p, program_state* prog)
{
	token_value* tok = get_token(p);

	if (tok->type == BREAK) {
		if (!prog->cur_iter_switch) {
			parse_error(tok, "break statement with no enclosing iterative or switch block\n");
		}
	} else if (!prog->cur_iter) {
		parse_error(tok, "continue statement with no enclosing iterative block\n");
	}


	statement cont_or_break;
	memset(&cont_or_break, 0, sizeof(statement));
	cont_or_break.type = (tok->type == BREAK) ? BREAK_STMT : CONTINUE_STMT;
	cont_or_break.parent = prog->cur_parent;
	cvec_push_void(prog->stmt_list, &cont_or_break);

	tok = get_token(p);
	if (tok->type != SEMICOLON) {
		parse_error(tok, "in break_or_continue statement, expected SEMICOLON\n");
	}
}

void for_stmt(parsing_state* p, program_state* prog)
{
	get_token(p); //  (

	token_value* tok = get_token(p);
	if (tok->type != LPAREN) {
		parse_error(tok, "in for_stmt, LPAREN expected\n");
	}

	statement a_stmt;
	memset(&a_stmt, 0, sizeof(statement));

	//open enclosing block scope
	a_stmt.parent = prog->cur_parent;
	a_stmt.type = START_COMPOUND_STMT;
	a_stmt.bindings = cvec_void_heap(0, 3, sizeof(binding), NULL, NULL);

	int old_parent = prog->cur_parent;
	cvector_void* old_bindings = prog->bindings;

	prog->cur_parent = prog->stmt_list->size;
	prog->bindings = a_stmt.bindings;
	cvec_push_void(prog->stmt_list, &a_stmt);

	//initial clause
	if (peek_token(p, 0)->type != SEMICOLON) {
		var_type vtype = declaration_specifier(p, prog, 0, NULL);
		if (vtype != UNKNOWN) {
			declaration(p, prog);
			parse_seek(p, SEEK_CUR, -1); //declaration ate ';' so back up one
		} else {
			memset(&a_stmt, 0, sizeof(statement));

			a_stmt.parent = prog->cur_parent;
			a_stmt.exp = make_expression(prog);
			a_stmt.type = EXPR_STMT;

			expr(p, prog, a_stmt.exp);
			cvec_push_void(prog->stmt_list, &a_stmt);
		}
	}

	get_token(p); //first ;

	//for statement and middle clause
	memset(&a_stmt, 0, sizeof(statement));
	a_stmt.parent = prog->cur_parent;
	a_stmt.type = FOR_STMT;
	if (peek_token(p, 0)->type != SEMICOLON) {
		a_stmt.exp = make_expression(prog);
		expr(p, prog, a_stmt.exp);
	}
	size_t for_loc = prog->stmt_list->size;
	cvec_push_void(prog->stmt_list, &a_stmt);

	get_token(p); //second ;

	//3rd clause
	memset(&a_stmt, 0, sizeof(statement));
	a_stmt.parent = prog->cur_parent;
	if (peek_token(p, 0)->type != RPAREN) {
		a_stmt.exp = make_expression(prog);
		a_stmt.type = EXPR_STMT;
		expr(p, prog, a_stmt.exp);
	}

	get_token(p);  // )

	int old_cur_iter = prog->cur_iter;
	int old_cur_i_switch = prog->cur_iter_switch;

	prog->cur_iter = prog->cur_iter_switch = for_loc;

	statement_rule(p, prog);

	//TODO more efficient way to do this
	binding* b;
	for (int i=0; i<prog->bindings->size; ++i) {
		b = GET_BINDING(prog->bindings, i);
		remove_binding(prog, b->name);
	}

	prog->cur_iter = old_cur_iter;
	prog->cur_iter_switch = old_cur_i_switch;
	prog->bindings = old_bindings;

	//put 3rd expression "increment clause" at end
	size_t third_expr = prog->stmt_list->size;
	if (a_stmt.exp)
		cvec_push_void(prog->stmt_list, &a_stmt);

	statement a_goto;
	memset(&a_goto, 0, sizeof(statement));
	a_goto.type = GOTO_STMT;
	a_goto.parent = prog->cur_parent;
	a_goto.jump_to = for_loc;
	cvec_push_void(prog->stmt_list, &a_goto);

	GET_STMT(prog->stmt_list, for_loc)->jump_to = prog->stmt_list->size;

	//close enclosing block scope
	memset(&a_stmt, 0, sizeof(statement));
	a_stmt.parent = prog->cur_parent;
	a_stmt.type = END_COMPOUND_STMT;
	cvec_push_void(prog->stmt_list, &a_stmt);

	prog->cur_parent = old_parent;

	statement* stmt;
	for (int i=for_loc+1; i<prog->stmt_list->size-3; ++i) {
		stmt = GET_STMT(prog->stmt_list, i);
		if (stmt->type == BREAK_STMT && stmt->jump_to == 0) {
			stmt->type = GOTO_STMT;
			stmt->jump_to =  prog->stmt_list->size;
		} else if (stmt->type == CONTINUE_STMT && stmt->jump_to == 0) {
			stmt->type = GOTO_STMT;
			stmt->jump_to = third_expr;
		}
	}
}

void goto_stmt(parsing_state* p, program_state* prog)
{
	token_value* tok;
	get_token(p); //get 'goto'

	statement a_goto;
	memset(&a_goto, 0, sizeof(statement));
	a_goto.type = GOTO_STMT;
	a_goto.parent = prog->cur_parent;

	tok = get_token(p);
	if (tok->type != ID) {
		parse_error(tok, "in goto_stmt, expected ID (for label)\n");
	}

	a_goto.lvalue = tok->v.id;

	tok = get_token(p);
	if (tok->type != SEMICOLON) {
		parse_error(tok, "in goto_stmt, expected SEMICOLON\n");
	}

	cvec_push_void(prog->stmt_list, &a_goto); //jump to after else statement
}

void labeled_stmt(parsing_state* p, program_state* prog)
{
	token_value* tok = get_token(p);
	get_token(p); //get ':'

	cvec_push_str(&prog->func->labels, tok->v.id);
	cvec_push_i(&prog->func->label_locs, prog->stmt_list->size);

	statement_rule(p, prog);
}

void return_stmt(parsing_state* p, program_state* prog)
{
	get_token(p); //eat 'return'
	statement ret_stmt;
	memset(&ret_stmt, 0, sizeof(statement));

	ret_stmt.parent = prog->cur_parent;
	ret_stmt.type = RETURN_STMT;

	if (peek_token(p, 0)->type != SEMICOLON) {
		if (prog->func->ret_val.type == VOID_TYPE) {
			parse_error(peek_token(p, 0), "return statement with an expression in a function with void return type\n");
		}
		ret_stmt.exp = make_expression(prog);
		expr(p, prog, ret_stmt.exp);
	} else if (prog->func->ret_val.type != VOID_TYPE) {
		parse_error(peek_token(p, 0), "return statement with no expression in a function with a return type\n");
	}

	cvec_push_void(prog->stmt_list, &ret_stmt);

	get_token(p); //eat ;
}

void expression_stmt(parsing_state* p, program_state* prog)
{
	statement an_expr;
	memset(&an_expr, 0, sizeof(statement));

	an_expr.parent = prog->cur_parent;
	an_expr.exp = make_expression(prog);
	an_expr.type = EXPR_STMT;

	expr(p, prog, an_expr.exp);

	cvec_push_void(prog->stmt_list, &an_expr);


	token_value* tok = get_token(p);
	if (tok->type != SEMICOLON) {
		parse_error(tok, "in expression_stmt, SEMICOLON expected\n");
	}
}

void expr(parsing_state* p, program_state* prog, expression* e)
{
	comma_expr(p, prog, e);
}

void comma_expr(parsing_state* p, program_state* prog, expression* e)
{
	assign_expr(p, prog, e);

	token_value* tok = peek_token(p, 0);
	while (tok->type == COMMA) {
		get_token(p);

		e->left = copy_expr(prog, e);
		e->tok.type = tok->type;

		e->right = make_expression(prog);

		assign_expr(p, prog, e->right);
		tok = peek_token(p, 0);
	}
}


int assignment_operator(Token tok)
{
	return (tok == EQUAL || tok == ADDEQUAL || tok == SUBEQUAL ||
	        tok == MULTEQUAL || tok == DIVEQUAL || tok == MODEQUAL ||
			tok == BIT_AND_EQUAL || tok == BIT_OR_EQUAL ||
			tok == BIT_XOR_EQUAL || tok == LSHIFT_EQUAL || tok == RSHIFT_EQUAL);
}

/* assign_expr -> cond_expr | identifer assign_op assign_expr */
void assign_expr(parsing_state* p, program_state* prog, expression* e)
{
	token_value* tok;

	if (peek_token(p, 0)->type != ID) {
		cond_expr(p, prog, e);

		if (assignment_operator(peek_token(p, 0)->type)) {
			if (e->tok.type != ID) {  //TODO for now
				parse_error(&e->tok, "in assign_expr, lvalue required as left operand of assignment.\n");
			}
		}
		return;
	}

	tok = peek_token(p, 0);
	if (prog->func) {
		var_value* check = look_up_value(prog, tok->v.id, BOTH);
		if (!check) {
			parse_error(tok, "in assign_expr, undeclared variable\n");
		}
	}

	if (!assignment_operator(peek_token(p, 1)->type)) {
		cond_expr(p, prog, e);

		if (assignment_operator(peek_token(p, 0)->type)) {
			if (e->tok.type != ID) {  //TODO for now
				parse_error(&e->tok, "in assign_expr, lvalue required as left operand of assignment.\n");
			}
		}
		return;
	}

	get_token(p);  //match ID
	e->tok = *tok; //id from above

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
	//(apparently that method is called tail recursion ...)
	//
	//Here the top level left side never changes and only the right
	//side is expanded in the recursive call
	e->left = copy_expr(prog, e);
	e->tok.type = tok->type;

	e->right = make_expression(prog);

	assign_expr(p, prog, e->right);
}

expression* copy_expr(program_state* prog, expression* e)
{
	expression* copy = make_expression(prog);
	memcpy(copy, e, sizeof(expression));
	return copy;
}

void cond_expr(parsing_state* p, program_state* prog, expression* e)
{
	logical_or_expr(p, prog, e);

	if (peek_token(p, 0)->type == TERNARY) {
		get_token(p);
		e->left = copy_expr(prog, e);
		e->tok.type = TERNARY;
		e->right = make_expression(prog);

		e->right->left = make_expression(prog);
		expr(p, prog, e->right->left);
		if (peek_token(p, 0)->type != COLON) {
			parse_error(peek_token(p, 0), "in ternary expression expected ':'");
		}
		get_token(p);
		e->right->tok.type = COLON;
		e->right->right = make_expression(prog);
		cond_expr(p, prog, e->right->right);
	}
}

/* logical_or_expr -> logical_and_expr { '||' logical_and_expr } */
void logical_or_expr(parsing_state* p, program_state* prog, expression* e)
{
	logical_and_expr(p, prog, e);

	while (peek_token(p, 0)->type == LOGICAL_OR) {
		get_token(p);

		e->left = copy_expr(prog, e);
		e->tok.type = LOGICAL_OR;

		e->right = make_expression(prog);

		logical_and_expr(p, prog, e->right);
	}
}


/* logical_and_expr -> bitwise_or_expr { '&&' bitwise_or_expr } */
void logical_and_expr(parsing_state* p, program_state* prog, expression* e)
{
	bitwise_or_expr(p, prog, e);

	while (peek_token(p, 0)->type == LOGICAL_AND) {
		get_token(p);

		e->left = copy_expr(prog, e);
		e->tok.type = LOGICAL_AND;

		e->right = make_expression(prog);

		bitwise_or_expr(p, prog, e->right);
	}
}


/* bitwise_or_expr -> bitwise_xor_expr { '|' bitwise_xor_expr } */
void bitwise_or_expr(parsing_state* p, program_state* prog, expression* e)
{
	bitwise_xor_expr(p, prog, e);

	while (peek_token(p, 0)->type == BIT_OR) {
		get_token(p);

		e->left = copy_expr(prog, e);
		e->tok.type = BIT_OR;

		e->right = make_expression(prog);

		bitwise_xor_expr(p, prog, e->right);
	}
}

/* bitwise_xor_expr -> bitwise_and_expr { '^' bitwise_and_expr } */
void bitwise_xor_expr(parsing_state* p, program_state* prog, expression* e)
{
	bitwise_and_expr(p, prog, e);

	while (peek_token(p, 0)->type == BIT_XOR) {
		get_token(p);

		e->left = copy_expr(prog, e);
		e->tok.type = BIT_XOR;

		e->right = make_expression(prog);

		bitwise_and_expr(p, prog, e->right);
	}
}

/* bitwise_and_expr -> equality_expr { '&' equality_expr } */
void bitwise_and_expr(parsing_state* p, program_state* prog, expression* e)
{
	equality_expr(p, prog, e);

	while (peek_token(p, 0)->type == BIT_AND) {
		get_token(p);

		e->left = copy_expr(prog, e);
		e->tok.type = BIT_AND;

		e->right = make_expression(prog);

		equality_expr(p, prog, e->right);
	}
}

/* equality_expr -> relational_expr { '=='|'!=' relational_expr } */
void equality_expr(parsing_state* p, program_state* prog, expression* e)
{
	relational_expr(p, prog, e);

	token_value* tok = peek_token(p, 0);
	while (tok->type == EQUALEQUAL || tok->type == NOTEQUAL) {
		get_token(p);

		e->left = copy_expr(prog, e);
		e->tok.type = tok->type;

		e->right = make_expression(prog);

		relational_expr(p, prog, e->right);
		tok = peek_token(p, 0);
	}
}

/* relational_expr -> shift_expr { relational_op shift_expr } */
void relational_expr(parsing_state* p, program_state* prog, expression* e)
{
	shift_expr(p, prog, e);

	token_value* tok = peek_token(p, 0);
	while (tok->type == GREATER || tok->type == GTEQ || tok->type == LESS || tok->type == LTEQ) {
		get_token(p);
		e->left = copy_expr(prog, e);
		e->tok.type = tok->type;

		e->right = make_expression(prog);
		shift_expr(p, prog, e->right);

		tok = peek_token(p, 0);
	}
}

/* shift_expr -> add_expr { shift_op add_expr } */
void shift_expr(parsing_state* p, program_state* prog, expression* e)
{
	add_expr(p, prog, e);

	token_value* tok = peek_token(p, 0);
	while (tok->type == LEFT_SHIFT || tok->type == RIGHT_SHIFT) {
		get_token(p);
		e->left = copy_expr(prog, e);
		e->tok.type = tok->type;

		e->right = make_expression(prog);
		add_expr(p, prog, e->right);

		tok = peek_token(p, 0);
	}
}


/* add_expr -> mult_expr { '+'|'-' mult_expr } */
void add_expr(parsing_state* p, program_state* prog, expression* e)
{
	mult_expr(p, prog, e);

	token_value* tok = peek_token(p, 0);
	while (tok->type == ADD || tok->type == SUB) {
		get_token(p);

		e->left = copy_expr(prog, e);
		e->tok.type = tok->type;

		e->right = make_expression(prog);
		mult_expr(p, prog, e->right);

		tok = peek_token(p, 0);
	}
}

/* mult_expr ->  unary_expr { '*'|'/'|'%' unary_expr } */
void mult_expr(parsing_state* p, program_state* prog, expression* e)
{
	token_value* tok;

	unary_expr(p, prog, e);

	tok = peek_token(p, 0);
	while (tok->type == MULT || tok->type == DIV || tok->type == MOD) {
		get_token(p);

		e->left = copy_expr(prog, e);
		e->tok.type = tok->type;

		e->right = make_expression(prog);
		unary_expr(p, prog, e->right);

		tok = peek_token(p, 0);
	}
}

/* unary_expr -> unary+ | unary- | logical_neg | bit_neg | postfix_expr |
 *               ++pre | --pre | TODO sizeof_expr */
void unary_expr(parsing_state* p, program_state* prog, expression* e)
{
	switch (peek_token(p, 0)->type) {
	case ADD:   //unary +, -
	case SUB:
	case LOGICAL_NEGATION:
	case BIT_NEGATION:
		e->tok.type = get_token(p)->type; //match
		e->left = make_expression(prog);

		unary_expr(p, prog, e->left);
		break;

	case INCREMENT:
	case DECREMENT:
		pre_inc_decrement_expr(p, prog, e);
		break;

	case SIZEOF:
		sizeof_expr(p, prog, e);
		break;


	default:
		postfix_expr(p, prog, e);
	}
}

/* sizeof_expr -> sizeof ( type-name ) | sizeof unary-expr */
void sizeof_expr(parsing_state* p, program_state* prog, expression* e)
{
	get_token(p); // get sizeof

	Token t0 = peek_token(p, 0)->type;
	Token t1 = peek_token(p, 1)->type;

	if (t0 != LPAREN || t1 < CHAR || t1 > VOID) {
		// TODO this is bad, sizeof is supposed to be compile time
		// but I'm treating it like a runtime operator...
		//
		// I'll figure a better way about the same time I extract the expression
		// parsing tree into a form that I can reuse for the preprocessor
		e->tok.type = SIZEOF;
		e->left = make_expression(prog);
		unary_expr(p, prog, e->left);
	} else {

		if (t0 != LPAREN) {
			parse_error(get_token(p), "LPAREN expected after sizeof\n");
		}
		get_token(p); // eat (

		// TODO handle pointers, typedef'd types, structs/unions etc.
		// and qualifed types like "unsigned short"
		if (t1 >= CHAR && t1 <= VOID) {
			get_token(p); // eat type

			// technically sizeof returns size_t which is unsigned ...
			e->tok.type = INT_LITERAL;
			switch (t1) {
			case CHAR: e->tok.v.int_val = 1; break;
			case SHORT: e->tok.v.int_val = sizeof(short); break;
			case INT: e->tok.v.int_val = sizeof(int); break;
			case LONG: e->tok.v.int_val = sizeof(long); break;
			case FLOAT: e->tok.v.int_val = sizeof(float); break;
			case DOUBLE: e->tok.v.int_val = sizeof(double); break;

			// nest another switch in these to get qualified types...
			case SIGNED: e->tok.v.int_val = sizeof(signed); break;
			case UNSIGNED: e->tok.v.int_val = sizeof(unsigned); break;

			// TODO give warning
			// non-standard, matches gcc and clang, gives a warning
			// with -pedantic (-Wall gives no warning)
			case VOID: e->tok.v.int_val = 1; break;
			}

			token_value* tok = get_token(p);
			if (tok->type != RPAREN) {
				parse_error(tok, "RPAREN expected to close sizeof\n");
			}
		} else {
			parse_error(get_token(p), "type name expected in sizeof\n");
		}
	}
}

void pre_inc_decrement_expr(parsing_state* p, program_state* prog, expression* e)
{
	token_value* tok = get_token(p);
	e->tok.type = (tok->type == INCREMENT) ? PRE_INCREMENT : PRE_DECREMENT;
	e->left = make_expression(prog);
	unary_expr(p, prog, e->left);

	// TODO This check will have to change later
	expression* tmp = e->left;
	while (tmp->tok.type != ID) {
		if (tmp->tok.type == EXP) {
			tmp = tmp->left;
		} else {
			parse_error(&tmp->tok, "lvalue required as increment or decrement operand\n");
		}
	}
}

/* postfix_expr -> function_call | primary_expr | post_inc | post_dec
 *           TODO  subscrpt_expr | component_selection_expr | compound_literal */
void postfix_expr(parsing_state* p, program_state* prog, expression* e)
{
	token_value* tok = peek_token(p, 0);
	if (tok->type == ID && peek_token(p, 1)->type == LPAREN) {
		function_call(p, prog, e);
	} else {
		primary_expr(p, prog, e);
		tok = peek_token(p, 0);
		if (tok->type == INCREMENT || tok->type == DECREMENT) {
			while (e->tok.type != ID) {
				if (e->tok.type == EXP) {
					e = e->left;
				} else {
					parse_error(&e->tok, "lvalue required as increment operand\n");
				}
			}
			e->left = copy_expr(prog, e);
			e->tok.type = (tok->type == INCREMENT) ? POST_INCREMENT : POST_DECREMENT;
			get_token(p);
		}
	}
}


void function_call(parsing_state* p, program_state* prog, expression* e)
{
	token_value* tok = get_token(p);
	get_token(p);  //get '('

	e->tok.type = FUNC_CALL;

	e->left = make_expression(prog);

	e->left->tok.type = ID;
	e->left->tok.v.id = tok->v.id;

	//this check will have to change when I allow function pointers
	var_value* check = look_up_value(prog, tok->v.id, ONLY_GLOBAL);
	if (!check) {
		parse_error(tok, "implicit declaration of function '%s'.\n", tok->v.id);
	} else if (check->type != FUNCTION_TYPE) {
		parse_error(tok, "called object '%s' is not a function\n", tok->v.id);
	}


	e->right = make_expression(prog);

	if (peek_token(p, 0)->type == RPAREN) {
		e->right->tok.type = VOID;
		get_token(p);
		return;
	}
	expression_list(p, prog, e->right);

	tok = get_token(p);
	if (tok->type != RPAREN) {
		parse_error(tok, "in function_call, ')' expected\n");
	}
}

void expression_list(parsing_state* p, program_state* prog, expression* e)
{
	assign_expr(p, prog, e);

	if (peek_token(p, 0)->type == COMMA) {
		get_token(p);

		e->left = copy_expr(prog, e);
		e->tok.type = EXPR_LIST;

		e->right = make_expression(prog);

		expression_list(p, prog, e->right);
	}
}

/* primary -> '(' expr ')' | ID | *_LITERAL */
void primary_expr(parsing_state* p, program_state* prog, expression* e)
{
	token_value* tok = get_token(p);

	switch (tok->type) {
	case LPAREN:
		e->tok.type = EXP;
		e->left = make_expression(prog);
		assert(e->left);
		expr(p, prog, e->left);

		tok = get_token(p);
		if (tok->type != RPAREN) {
			parse_error(tok, "in primary_expr, RPAREN expected.\n");
		}
		break;
	case ID:
		if (!prog->func) {
			parse_error(tok, "global variable initializer element is not a constant\n");
		} else {
			var_value* check = look_up_value(prog, tok->v.id, BOTH);
			if (!check) {
				parse_error(tok, "undeclared variable\n");
			}
		}
		e->tok.type = ID;
		e->tok.v.id = tok->v.id;
		break;
	case INT_LITERAL:
		e->tok.type = INT_LITERAL;
		e->tok.v.int_val = tok->v.int_val;
		break;
	case FLOAT_LITERAL:
		e->tok.type = FLOAT_LITERAL;
		e->tok.v.float_val = tok->v.float_val;
		break;
	case DOUBLE_LITERAL:
		e->tok.type = DOUBLE_LITERAL;
		e->tok.v.double_val = tok->v.double_val;
		break;
	default:
		parse_error(tok, "in primary_expr, LPAREN or literal expected\n");
	}
}



void if_stmt(parsing_state* p, program_state* prog)
{
	get_token(p); //get if

	statement an_if;
	memset(&an_if, 0, sizeof(statement));
	an_if.type = IF_STMT;
	an_if.parent = prog->cur_parent;

	an_if.exp = make_expression(prog);

	token_value* tok = get_token(p);
	if (tok->type != LPAREN) {
		parse_error(tok, "in if_stmt, expected LPAREN\n");
	}

	expr(p, prog, an_if.exp);


	tok = get_token(p);
	if (tok->type != RPAREN) {
		parse_error(tok, "in if_stmt, expected RPAREN\n");
	}

	size_t if_loc = prog->stmt_list->size;
	cvec_push_void(prog->stmt_list, &an_if);

	statement_rule(p, prog);

	GET_STMT(prog->stmt_list, if_loc)->jump_to = prog->stmt_list->size;

	tok = peek_token(p, 0);
	if (tok->type == ELSE) {
		get_token(p);

		statement a_goto;
		memset(&a_goto, 0, sizeof(statement));
		a_goto.type = GOTO_STMT;
		a_goto.parent = prog->cur_parent;
		size_t goto_loc = prog->stmt_list->size;
		cvec_push_void(prog->stmt_list, &a_goto); //jump to after else statement

		//reset jump for failed if condition to after the goto
		GET_STMT(prog->stmt_list, if_loc)->jump_to = prog->stmt_list->size;

		statement_rule(p, prog);

		GET_STMT(prog->stmt_list, goto_loc)->jump_to = prog->stmt_list->size;
	}
}


void print_stmt(parsing_state* p, program_state* prog)
{
	get_token(p); //get print

	statement a_print;
	memset(&a_print, 0, sizeof(statement));
	a_print.type = PRINT_STMT;
	a_print.parent = prog->cur_parent;
	a_print.exp = make_expression(prog);

	expr(p, prog, a_print.exp);

	//a_print.lvalue = tok->v.id;

	token_value* tok = get_token(p);
	if (tok->type != SEMICOLON) {
		parse_error(tok, "in print_stmt, SEMICOLON expected.\n");
	}

	cvec_push_void(prog->stmt_list, &a_print);
}



void while_stmt(parsing_state* p, program_state* prog)
{
	get_token(p); //get while

	statement a_while;
	memset(&a_while, 0, sizeof(statement));
	a_while.type = WHILE_STMT;
	a_while.parent = prog->cur_parent;

	a_while.exp = make_expression(prog);

	token_value* tok = get_token(p);
	if (tok->type != LPAREN) {
		parse_error(tok, "in while_stmt, expected LPAREN\n");
	}

	expr(p, prog, a_while.exp);

	tok = get_token(p);
	if (tok->type != RPAREN) {
		parse_error(tok, "in while_stmt, expected RPAREN\n");
	}

	size_t while_loc = prog->stmt_list->size;
	int old_cur_iter = prog->cur_iter;
	int old_cur_i_switch = prog->cur_iter_switch;

	prog->cur_iter = prog->cur_iter_switch = while_loc;
	cvec_push_void(prog->stmt_list, &a_while);

	statement_rule(p, prog);

	prog->cur_iter = old_cur_iter;
	prog->cur_iter_switch = old_cur_i_switch;

	statement a_goto;
	memset(&a_goto, 0, sizeof(statement));
	a_goto.type = GOTO_STMT;
	a_goto.parent = prog->cur_parent;
	a_goto.jump_to = while_loc;
	cvec_push_void(prog->stmt_list, &a_goto);

	GET_STMT(prog->stmt_list, while_loc)->jump_to = prog->stmt_list->size;

	statement* stmt;
	for (int i=while_loc+1; i<prog->stmt_list->size-1; ++i) {
		stmt = GET_STMT(prog->stmt_list, i);
		if (stmt->type == BREAK_STMT && stmt->jump_to == 0) {
			stmt->type = GOTO_STMT;
			stmt->jump_to =  prog->stmt_list->size;
		} else if (stmt->type == CONTINUE_STMT && stmt->jump_to == 0) {
			stmt->type = GOTO_STMT;
			stmt->jump_to = while_loc;
		}
	}
}



void parse_error(token_value* tok, char *str, ...)
{
	va_list args;
	va_start(args, str);
	fprintf(stderr, "Parse Error: ");
	vfprintf(stderr, str, args);
	if (tok) {
		if (str[strlen(str)-1] == '\n')
			fprintf(stderr, "Got ");
		else
			fprintf(stderr, ", got ");
		print_token(tok, stderr, 0);
		token_lex* lex = (token_lex*)tok;
		fprintf(stderr, " at line %u, position %u\n", lex->line, lex->pos);
	}
	va_end(args);

	exit(0);

}
