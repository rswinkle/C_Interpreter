#include "interpreter.h"
#include "c_utils.h"



void run(program_state* prog, char* start_func)
{
	char* func_name = (start_func) ? start_func : "main";
	var_value* var = look_up_value(prog, func_name, ONLY_GLOBAL); //global is current local
	if (!var) {
		fprintf(stderr, "Error: function '%s' not found.\n", func_name);
		exit(0);
	}
	function* func = GET_FUNCTION(&prog->functions, var->v.func_loc);
	prog->func = func;
	prog->stmt_list = &func->stmt_list;
	func->pc = 0;
	prog->pc = &func->pc;
	prog->cur_parent = -1;

	//set parameters here, ie argc, argv

	execute(prog);

	var_value ret = prog->func->ret_val;

	cvec_free_void(&prog->functions);
	cvec_free_str(&prog->global_variables);
	cvec_free_void(&prog->global_values);
	cvec_free_void(&prog->expressions);
	cvec_free_str(&prog->string_db);
}



void execute(program_state* prog)
{
	*prog->pc = 0;
	int i, outer_parent = prog->cur_parent;
	statement* stmt, *target;

	while (*prog->pc < prog->stmt_list->size) {
		stmt = GET_STMT(prog->stmt_list, *prog->pc);
		//print_statement(stmt);

		switch (stmt->type) {

		case DECL_STMT:
			add_binding(prog, stmt->lvalue, stmt->vtype);
			break;

		case PRINT_STMT:
			execute_print(execute_expr(prog, stmt->exp));
			break;

		case EXPR_STMT:
			execute_expr(prog, stmt->exp);
			break;

		case SWITCH_STMT:
		{
			var_value v = execute_expr(prog, stmt->exp);
			a_case* c;
			for (i=0; i<stmt->bindings->size; ++i) {
				c = CVEC_GET_VOID(stmt->bindings, a_case, i);
				if (c->val == v.v.int_val) {
					*prog->pc = c->jump_to - 1;
					break;
				}
			}
			if (i == stmt->bindings->size)
				*prog->pc = stmt->jump_to - 1;
		}
			break;

		case IF_STMT:
			if (!is_true(execute_expr(prog, stmt->exp)))
				*prog->pc = stmt->jump_to - 1; //could get rid of -1's and continue
			break;

		case WHILE_STMT:
			if (!is_true(execute_expr(prog, stmt->exp)))
				*prog->pc = stmt->jump_to - 1;
			break;

		case DO_STMT:
			if (is_true(execute_expr(prog, stmt->exp)))
				*prog->pc = stmt->jump_to - 1;
			break;

		case FOR_STMT:
			if (stmt->exp && !is_true(execute_expr(prog, stmt->exp)))
				*prog->pc = stmt->jump_to - 1;
			break;

		case GOTO_STMT:
			execute_goto(prog, stmt);
			*prog->pc = stmt->jump_to-1;
			break;

		case RETURN_STMT:
			{
				var_value result, tmp;
				if (prog->func->ret_val.type != VOID_TYPE) {
					tmp = execute_expr(prog, stmt->exp);
					BINARY_OP((&prog->func->ret_val), =, (&tmp));
				}
			}

			clear_bindings(prog);
			prog->cur_parent = outer_parent;
			return;

		case START_COMPOUND_STMT:
			prog->cur_parent = *prog->pc;
			break;

		case END_COMPOUND_STMT:
			pop_scope(prog);	//freeing all bindings with cur_parent
			prog->cur_parent = GET_STMT(prog->stmt_list, stmt->parent)->parent;
			break;

		case CASE_STMT:
		case NULL_STMT:
			break;

		default:
			fprintf(stderr, "Interpreter error: unrecognized statement type.\n\n");
			exit(0);
		}

		(*prog->pc)++;
	}

	if (prog->func->ret_val.type != VOID_TYPE) {
		fprintf(stderr, "Warning, falling off the end of non-void function.\n");
	}
}

int is_true(var_value v)
{
	switch (v.type) {
	case CHAR_TYPE:     return (v.v.char_val != 0);
	case UCHAR_TYPE:    return (v.v.uchar_val != 0);
	case SHORT_TYPE:    return (v.v.short_val != 0);
	case USHORT_TYPE:   return (v.v.ushort_val != 0);
	case INT_TYPE:      return (v.v.int_val != 0);
	case UINT_TYPE:     return (v.v.uint_val != 0);
	case LONG_TYPE:     return (v.v.long_val != 0);
	case ULONG_TYPE:    return (v.v.ulong_val != 0);
	case FLOAT_TYPE:    return (v.v.float_val) ? 1 : 0; //required for correct behavior
	case DOUBLE_TYPE:   return (v.v.double_val) ? 1 : 0;//for floating types

	default:
		printf("%d\n", v.type);
		fprintf(stderr, "Error unknown type\n");
		exit(0);
	}
}



void execute_print(var_value a)
{
	switch (a.type) {
		case CHAR_TYPE:
			printf("%d\n", a.v.char_val); //use %c ?
			break;
		case UCHAR_TYPE:
			printf("%u\n", a.v.uchar_val);
			break;
		case SHORT_TYPE:
			printf("%d\n", a.v.short_val);
			break;
		case USHORT_TYPE:
			printf("%u\n", a.v.ushort_val);
			break;
		case INT_TYPE:
			printf("%d\n", a.v.int_val);
			break;
		case UINT_TYPE:
			printf("%u\n", a.v.uint_val);
			break;
		case LONG_TYPE:
			printf("%ld\n", a.v.long_val);
			break;
		case ULONG_TYPE:
			printf("%lu\n", a.v.ulong_val);
			break;
		case FLOAT_TYPE:
			printf("%f\n", a.v.float_val);
			break;
		case DOUBLE_TYPE:
			printf("%f\n", a.v.double_val);
			break;
		default:
			fprintf(stderr, "type not supported yet\n");
			exit(0);
	}
}

var_value execute_expr(program_state* prog, expression* e)
{
	var_value* var;

	function* old_func, *func;
	cvector_void* old_stmt_list;
	size_t* old_pc;

	var_value result, left, right;
	var_value *left_ptr = &left, *right_ptr = &right;

	switch (e->tok.type) {
	case EXP:          return execute_expr(prog, e->left);

//operations in highest to lowest precedence groups

	case FUNC_CALL:
		break;

	case POST_INCREMENT:
	case POST_DECREMENT:


	case PRE_INCREMENT:
	case PRE_DECREMENT:
		var = look_up_value(prog, e->left->tok.v.id, BOTH);
		break;

	case LOGICAL_NEGATION:
	case BIT_NEGATION:
		left = execute_expr(prog, e->left);
		break;


	case MULT:
	case DIV:
	case MOD:

	case ADD:
	case SUB:

	case LEFT_SHIFT:
	case RIGHT_SHIFT:

	case GREATER:
	case LESS:
	case GTEQ:
	case LTEQ:

	case NOTEQUAL:
	case EQUALEQUAL:

	case BIT_AND:

	case BIT_XOR:

	case BIT_OR:

	case LOGICAL_AND:

	case LOGICAL_OR:
		left = execute_expr(prog, e->left);
		right = execute_expr(prog, e->right);
		break;

	case TERNARY:
		left = execute_expr(prog, e->right->left);
		right = execute_expr(prog, e->right->right);
		break;

	case EQUAL:
	case ADDEQUAL:
	case SUBEQUAL:
	case MULTEQUAL:
	case DIVEQUAL:
	case MODEQUAL:
	case BIT_AND_EQUAL:
	case BIT_OR_EQUAL:
	case BIT_XOR_EQUAL:
	case LSHIFT_EQUAL:
	case RSHIFT_EQUAL:
		right = execute_expr(prog, e->right);
		var = look_up_value(prog, e->left->tok.v.id, BOTH);
		break;

	case COMMA:
		left = execute_expr(prog, e->left);
		right = execute_expr(prog, e->right);
		break;

		//bottom of precedence stack
		//you could argue ID's and literals are the top
		//of the stack but whatever, they're not operators

	case ID:
		var = look_up_value(prog, e->tok.v.id, BOTH);
		result = *var;
		return result;

	case INT_LITERAL:
		result.type = INT_TYPE;
		result.v.int_val = e->tok.v.int_val;
		return result;

	case FLOAT_LITERAL:
		result.type = FLOAT_TYPE;
		result.v.float_val = e->tok.v.float_val;
		return result;

	case DOUBLE_LITERAL:
		result.type = DOUBLE_TYPE;
		result.v.double_val = e->tok.v.double_val;
		return result;

	default:
		fprintf(stderr, "Interpreter error: Unrecognized op %d in expression.\n\n", e->tok.type);
		print_token(&e->tok, stderr, 0);
		exit(0);
	}



	switch (e->tok.type) {

//binary operations
	case ADD:             BINARY_OP(left_ptr, +, right_ptr);    break;
	case SUB:             BINARY_OP(left_ptr, -, right_ptr);    break;
	case MULT:            BINARY_OP(left_ptr, *, right_ptr);    break;
	case DIV:             BINARY_OP(left_ptr, /, right_ptr);    break;
	case MOD:             INTEGRAL_BINARY_OP(left_ptr, %, right_ptr);    break;

	case GREATER:         BINARY_OP(left_ptr, >, right_ptr);    break;
	case LESS:            BINARY_OP(left_ptr, <, right_ptr);    break;
	case GTEQ:            BINARY_OP(left_ptr, >=, right_ptr);    break;
	case LTEQ:            BINARY_OP(left_ptr, <=, right_ptr);    break;
	case NOTEQUAL:        BINARY_OP(left_ptr, !=, right_ptr);    break;
	case EQUALEQUAL:      BINARY_OP(left_ptr, ==, right_ptr);    break;

	case BIT_AND:         INTEGRAL_BINARY_OP(left_ptr, &, right_ptr);    break;
	case BIT_XOR:         INTEGRAL_BINARY_OP(left_ptr, ^, right_ptr);    break;
	case BIT_OR:          INTEGRAL_BINARY_OP(left_ptr, |, right_ptr);    break;

	case LOGICAL_OR:      BINARY_OP(left_ptr, ||, right_ptr);    break;
	case LOGICAL_AND:     BINARY_OP(left_ptr, &&, right_ptr);    break;

/* old method with dead side effectless left side
#define MYCOMMA ,
	case COMMA:           BINARY_OP(left_ptr, MYCOMMA, right_ptr);    break;
#undef MYCOMMA
*/
	case COMMA:           COMMA_OP(right_ptr);   break;


	case TERNARY:         COND_EXPR(e->left, left_ptr, right_ptr);    break;

	case PRE_INCREMENT:      PRE_OP(++, var);    break;
	case PRE_DECREMENT:      PRE_OP(--, var);    break;
	case POST_INCREMENT:     POST_OP(var, ++);    break;
	case POST_DECREMENT:     POST_OP(var, --);    break;


	case EQUAL:              BINARY_OP(var, =, right_ptr);   break;
	case ADDEQUAL:           BINARY_OP(var, +=, right_ptr);   break;
	case SUBEQUAL:           BINARY_OP(var, -=, right_ptr);   break;
	case MULTEQUAL:          BINARY_OP(var, *=, right_ptr);   break;
	case DIVEQUAL:           BINARY_OP(var, /=, right_ptr);   break;

	case BIT_AND_EQUAL:      INTEGRAL_BINARY_OP(var, &=, right_ptr);   break;
	case BIT_OR_EQUAL:       INTEGRAL_BINARY_OP(var, |=, right_ptr);   break;
	case BIT_XOR_EQUAL:      INTEGRAL_BINARY_OP(var, ^=, right_ptr);   break;
	case LSHIFT_EQUAL:       INTEGRAL_BINARY_OP(var, <<=, right_ptr);   break;
	case RSHIFT_EQUAL:       INTEGRAL_BINARY_OP(var, >>=, right_ptr);   break;

	case MODEQUAL:           INTEGRAL_BINARY_OP(var, %=, right_ptr);   break;


	case LOGICAL_NEGATION:   PRE_OP(!, left_ptr);   break;
	case BIT_NEGATION:       PRE_OP(~, left_ptr);   break;


	case FUNC_CALL:
		old_stmt_list = prog->stmt_list;
		old_pc = prog->pc;
		old_func = prog->func;

		//look_up_value should never return NULL here, parsing should catch all errors like that
		func = GET_FUNCTION(&prog->functions, look_up_value(prog, e->left->tok.v.id, ONLY_GLOBAL)->v.func_loc);

		if (func->n_params) //could also check e->right->tok.type = VOID
			execute_expr_list(prog, func, e->right);

		prog->func = func;
		prog->stmt_list = &func->stmt_list;
		prog->pc = &func->pc;

		execute(prog); //run program

		prog->stmt_list = old_stmt_list;
		prog->pc = old_pc;
		prog->func = old_func;

		return func->ret_val;

	default:
		fprintf(stderr, "Interpreter error: Unrecognized op %d in expression.\n\n", e->tok.type);
		print_token(&e->tok, stderr, 0);
		exit(0);
	}

	return result;
}

//TODO
var_value execute_constant_expr(program_state* prog, expression* e)
{
	var_value *var;

	var_value result, left, right;
	var_value *left_ptr = &left, *right_ptr = &right;

	switch (e->tok.type) {
	case EXP:          return execute_expr(prog, e->left);

//binary operations
	case ADD: case SUB: case MULT: case DIV: case MOD:

	case GREATER: case LESS: case GTEQ: case LTEQ:
	case NOTEQUAL: case EQUALEQUAL: case LOGICAL_OR:
	case LOGICAL_AND: case COMMA:

		left = execute_expr(prog, e->left);
		right = execute_expr(prog, e->right);

		break;


	case TERNARY:
		left = execute_expr(prog, e->right->left);
		right = execute_expr(prog, e->right->right);
		break;


	case PRE_INCREMENT:
	case PRE_DECREMENT:
	case POST_INCREMENT:
	case POST_DECREMENT:
		var = look_up_value(prog, e->left->tok.v.id, BOTH);
		break;

	case EQUAL:
	case ADDEQUAL:
	case SUBEQUAL:
	case MULTEQUAL:
	case DIVEQUAL:
	case MODEQUAL:
		right = execute_expr(prog, e->right);
		var = look_up_value(prog, e->left->tok.v.id, BOTH);
		break;

	case LOGICAL_NEGATION:
		left = execute_expr(prog, e->left);
		break;


	case INT_LITERAL:
		result.type = INT_TYPE;
		result.v.int_val = e->tok.v.int_val;
		return result;

	case FLOAT_LITERAL:
		result.type = FLOAT_TYPE;
		result.v.float_val = e->tok.v.float_val;
		return result;

	case DOUBLE_LITERAL:
		result.type = DOUBLE_TYPE;
		result.v.double_val = e->tok.v.double_val;
		return result;

	default:
		parse_error(&e->tok, "constant expression expected\n");
		exit(0);
	}


	switch (e->tok.type) {

//binary operations
	case ADD:             BINARY_OP(left_ptr, +, right_ptr);    break;
	case SUB:             BINARY_OP(left_ptr, -, right_ptr);    break;
	case MULT:            BINARY_OP(left_ptr, *, right_ptr);    break;
	case DIV:             BINARY_OP(left_ptr, /, right_ptr);    break;
	case MOD:             INTEGRAL_BINARY_OP(left_ptr, %, right_ptr);    break;

	case GREATER:         BINARY_OP(left_ptr, >, right_ptr);    break;
	case LESS:            BINARY_OP(left_ptr, <, right_ptr);    break;
	case GTEQ:            BINARY_OP(left_ptr, >=, right_ptr);    break;
	case LTEQ:            BINARY_OP(left_ptr, <=, right_ptr);    break;
	case NOTEQUAL:        BINARY_OP(left_ptr, !=, right_ptr);    break;
	case EQUALEQUAL:      BINARY_OP(left_ptr, ==, right_ptr);    break;
	case LOGICAL_OR:      BINARY_OP(left_ptr, ||, right_ptr);    break;
	case LOGICAL_AND:     BINARY_OP(left_ptr, &&, right_ptr);    break;

/* according to pg 251 this (comma/sequential operator) shouldn't be here, see ./tests/switch.c
   old method with dead side effectless left side
#define MYCOMMA ,
	case COMMA:           BINARY_OP(left_ptr, MYCOMMA, right_ptr);    break;
#undef MYCOMMA
*/
	case COMMA:           COMMA_OP(right_ptr);   break;

	case TERNARY:         COND_EXPR(e->left, left_ptr, right_ptr);    break;

	case PRE_INCREMENT:      PRE_OP(++, var);    break;
	case PRE_DECREMENT:      PRE_OP(--, var);    break;
	case POST_INCREMENT:     POST_OP(var, ++);    break;
	case POST_DECREMENT:     POST_OP(var, --);    break;


	case EQUAL:              BINARY_OP(var, =, right_ptr);   break;
	case ADDEQUAL:           BINARY_OP(var, +=, right_ptr);   break;
	case SUBEQUAL:           BINARY_OP(var, -=, right_ptr);   break;
	case MULTEQUAL:          BINARY_OP(var, *=, right_ptr);   break;
	case DIVEQUAL:           BINARY_OP(var, /=, right_ptr);   break;
	case MODEQUAL:           INTEGRAL_BINARY_OP(var, %=, right_ptr);   break;


	case LOGICAL_NEGATION:   PRE_OP(!, left_ptr);   break;

	default:
		parse_error(&e->tok, "constant expression expected\n");
		exit(0);
	}
	return result;
}



/* executes parameter list of function call (aka expression_list)
 * from left to right assigning them to ascending parameters
 * order of evaluation is unspecified/implementation specific according to C spec
 *
 * gcc seems to do it in reverse order but tcc matches my output
 *
 */
void execute_expr_list(program_state* prog, function* callee, expression* e)
{
	function* func = callee;
	symbol* s;
	active_binding* v = malloc(sizeof(active_binding));;
	if (!v) {
		fprintf(stderr, "allocation failure in execute_expr_list\n");
		exit(0);
	}

	var_value result; // <-- necessary for macros but not used ... should refactor
	var_value val, *val_ptr = &val;

	int i = 0;
	while (e->tok.type == EXPR_LIST) {
		s = GET_SYMBOL(&func->symbols, i);

		val = execute_expr(prog, e->left);

		//this is really stupid ... maybe I should separate parameters from other bindings
		//a binding is allocated in parsing with the correct type ... so here we grab the type
		//of the last element.  It doesn't matter if it's recursive function and it calls itself
		//from a block where the symbol has another type because the "last" element
		//is always that unused binding put in during parsing with the original type of the parameter
		v->val.type = (list_entry(s->head.prev, active_binding, list))->val.type;
		BINARY_OP((&v->val), =, val_ptr);

		list_add(&v->list, &s->head);
		s->cur_parent = v->parent = 0; //reset for recursive call

		e = e->right;
		++i;

		if (!(v = malloc(sizeof(active_binding)))) {
			fprintf(stderr, "allocation failure in execute_expr_list\n");
			exit(0);
		}
	}

	s = GET_SYMBOL(&func->symbols, i);

	val = execute_expr(prog, e);
	v->val.type = (list_entry(s->head.prev, active_binding, list))->val.type;
	BINARY_OP((&v->val), =, val_ptr);

	list_add(&v->list, &s->head);
	s->cur_parent = v->parent = 0;
}


void execute_goto(program_state* prog, statement* stmt)
{
	int ancestor;
	statement* target = GET_STMT(prog->stmt_list, stmt->jump_to);
	if (stmt->parent != target->parent) {
		if (is_ancestor(prog, stmt->parent, target->parent)) {
			apply_scope(prog, stmt->jump_to, target->parent, stmt->parent);
		} else if (is_ancestor(prog, target->parent, stmt->parent)) {
			remove_scope(prog, stmt->jump_to, stmt->parent, target->parent);
		} else {
			ancestor = find_lowest_common_ancestor(prog, stmt->parent, target->parent);
			remove_scope(prog, stmt->jump_to, stmt->parent, ancestor);
			apply_scope(prog, stmt->jump_to, target->parent, ancestor);
		}
	} else {
		if (stmt->jump_to > *prog->pc) {
			binding* b;
			target = GET_STMT(prog->stmt_list, stmt->parent);
			for (int i=0; i<target->bindings->size; ++i) {
				b = GET_BINDING(target->bindings, i);
				if (b->decl_stmt > *prog->pc) {
					if (b->decl_stmt < stmt->jump_to) {
						add_binding(prog, b->name, b->vtype);
					} else {
						break;
					}
				}
			}
		}
	}
}




void add_binding(program_state* prog, char* name, var_type vtype)
{
	//make this calloc to get rid of valgrind warnings about condition
	//depends on uninitialized values (inside printf)
	//it's unspecified behavior anyway
	active_binding* value = calloc(1, sizeof(active_binding)); assert(value);
	value->val.type = vtype;
	value->parent = prog->cur_parent;

	symbol* s = look_up_symbol(prog, name);

	list_add(&value->list, &s->head);
	s->cur_parent = value->parent;
}

void remove_binding_symbol(symbol* s)
{
	active_binding* v = list_entry(s->head.next, active_binding, list);
	list_del(&v->list);
	free(v);

	if (!list_empty(&s->head)) {
		v = list_entry(s->head.next, active_binding, list);
		s->cur_parent = v->parent;
	} else {
		s->cur_parent = -1;
	}
}

void remove_binding(program_state* prog, char* name)
{
	symbol* s = look_up_symbol(prog, name);
	remove_binding_symbol(s);
}

//clear all bindings of current function call
void clear_bindings(program_state* prog)
{
	statement* stmt;
	binding* b;

	while (prog->cur_parent >= 0) {
		stmt = GET_STMT(prog->stmt_list, prog->cur_parent);
		for (int i=0; i<stmt->bindings->size; ++i) {
			b = GET_BINDING(stmt->bindings, i);
			if (b->decl_stmt > *prog->pc)
				break;
			remove_binding(prog, b->name);
		}

		prog->cur_parent = stmt->parent;
	}

	for (int i=0; i<prog->func->n_params; ++i) {
		remove_binding_symbol(GET_SYMBOL(&prog->func->symbols, i));
	}
}


void pop_scope(program_state* prog)
{
	symbol* s;

	for (int i=0; i<prog->func->symbols.size; ++i) {
		s = GET_SYMBOL(&prog->func->symbols, i);
		if (s->cur_parent == prog->cur_parent) {
			remove_binding_symbol(s);
		}
	}
}

//returns non-zero if parent is ancestor of child
int is_ancestor(program_state* prog, int parent, int child)
{
	statement* stmt;
	int tmp = child;
	do {
		stmt = GET_STMT(prog->stmt_list, tmp);
		tmp = stmt->parent;
	} while (tmp >= 0 && tmp != parent);

	return tmp == parent;
}

//apply scopes recursively from current scope (parent) to child scope up
//to jump_to statement
void apply_scope(program_state* prog, int jump_to, int child, int parent)
{
	statement* stmt = GET_STMT(prog->stmt_list, child);
	binding* b;

	if (child != parent)
		apply_scope(prog, jump_to, stmt->parent, parent);

	prog->cur_parent = child;
	//even once we get back to parent we have to check for jumping over decls
	for (int i=0; i<stmt->bindings->size; ++i) {
		b = GET_BINDING(stmt->bindings, i);
		if (b->decl_stmt < jump_to) {
			if (child != parent)
				add_binding(prog, b->name, b->vtype);
			else if (b->decl_stmt > *prog->pc)
				add_binding(prog, b->name, b->vtype);
		} else if (b->decl_stmt < *prog->pc && b->decl_stmt > jump_to) {
			remove_binding(prog, b->name);
		}
	}
}

//remove scope from child up to parent as of jump_to
void remove_scope(program_state* prog, int jump_to, int child, int parent)
{
	statement* stmt;
	binding *b;

	while (child != parent) {
		stmt = GET_STMT(prog->stmt_list, child);
		child = stmt->parent;
		pop_scope(prog);
	}

	prog->cur_parent = parent;

	stmt = GET_STMT(prog->stmt_list, parent);

	for (int i=0; i<stmt->bindings->size; ++i) {
		b = GET_BINDING(stmt->bindings, i);
		if (b->decl_stmt < jump_to && b->decl_stmt > *prog->pc) //jumping over a decl
			add_binding(prog, b->name, b->vtype);
		else if (b->decl_stmt > jump_to && b->decl_stmt < *prog->pc) //jumping back before a decl
			remove_binding(prog, b->name);
	}
}

int find_lowest_common_ancestor(program_state* prog, int parent1, int parent2)
{
	statement* stmt1, *stmt2;

	//I can't think of a better way right now so just brute forcing it
	//parent stmt/block with highest index is lowest common ancestor
	int max = 0;
	stmt1 = GET_STMT(prog->stmt_list, parent1);
	stmt2 = GET_STMT(prog->stmt_list, parent2);
	for (statement* s1 = stmt1; s1 != (statement*)prog->stmt_list->a; s1 = GET_STMT(prog->stmt_list, s1->parent)) {
		for (statement* s2 = stmt1; s2 != (statement*)prog->stmt_list->a ; s2 = GET_STMT(prog->stmt_list, s2->parent)) {
			if (s1->parent == s2->parent) {
				max = (s1->parent > max) ? s1->parent : max;
			}
		}
	}
	return max;
}

unsigned int look_up_loc(program_state* prog, const char* var)
{
	for (int i=0; i<prog->global_variables.size; ++i) {
		if (!strcmp(var, prog->global_variables.a[i]))
			return i;
	}

	fprintf(stderr, "Interpreter error: undeclared variable '%s'\n", var);
	exit(0);
}


symbol* look_up_symbol(program_state* prog, const char* var)
{
	symbol* v;
	for (int i=0; i<prog->func->symbols.size; ++i) {
		v = GET_SYMBOL(&prog->func->symbols, i);
		if (!strcmp(var, v->name))
			return v;
	}
	return NULL;
}



var_value* look_up_value(program_state* prog, const char* var, int search)
{
	symbol* s;
	if (prog->func && search != ONLY_GLOBAL) {
		for (int i=0; i<prog->func->symbols.size; ++i) {
			s = GET_SYMBOL(&prog->func->symbols, i);
			if (!strcmp(var, s->name)) {
				if (!list_empty(&s->head))
					//->next is first item, top of binding stack
					return &(list_entry(s->head.next, active_binding, list))->val;
 				else
					break;
			}
		}
	}

	if (search != ONLY_LOCAL || !prog->func) {  //if func == NULL global scope is local
		for (int i=0; i<prog->global_variables.size; ++i) {
			if (!strcmp(var, prog->global_variables.a[i]))
				return GET_VAR_VALUE(&prog->global_values, i);
		}
	}

	//Will never get here when interpreting, only when parsing to check if
	//something is undefined or redeclared.  IOW, there
	//is no danger of a NULL dereference above
	return NULL;
}










