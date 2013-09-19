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
	
	free_vec_void(&prog->functions);
	free_vec_str(&prog->global_variables);
	free_vec_void(&prog->global_values);
	free_vec_void(&prog->expressions);
	free_vec_str(&prog->string_db);
}



void execute(program_state* prog)
{
	*prog->pc = 0;
	int i, outer_parent = prog->cur_parent;
	statement* stmt, *target;

	while ( *prog->pc < prog->stmt_list->size ) {
		stmt = GET_STMT(prog->stmt_list, *prog->pc);

		switch (stmt->type) {

		case DECL_STMT:
			add_binding(prog, stmt->lvalue, stmt->vtype);
			break;

		case PRINT_STMT:
			//TODO print expression
			printf("%d\n", look_up_value(prog, stmt->lvalue, BOTH)->v.int_val);
			break;

		case EXPR_STMT:
			execute_expr(prog, stmt->exp);
			break;

		case IF_STMT:
			if (!execute_expr(prog, stmt->exp))
				*prog->pc = stmt->jump_to - 1;
			break;

		case WHILE_STMT:
			if (!execute_expr(prog, stmt->exp))
				*prog->pc = stmt->jump_to - 1;
			break;

		case FOR_STMT:
			if (stmt->exp && !execute_expr(prog, stmt->exp))
				*prog->pc = stmt->jump_to - 1;
			break;

		case GOTO_STMT:
			target = GET_STMT(prog->stmt_list, stmt->jump_to);
			if (stmt->parent != target->parent) {
				if (is_ancestor(prog, stmt->parent, target->parent)) {
					//puts("jumping to child");
					apply_scope(prog, stmt->jump_to, target->parent, stmt->parent);
				} else if (is_ancestor(prog, target->parent, stmt->parent)) {
					remove_scope(prog, stmt->jump_to, stmt->parent, target->parent);

				} else {
					//puts("siblings\n");
					int ancestor = find_lowest_common_ancestor(prog, stmt->parent, target->parent);
					remove_scope(prog, stmt->jump_to, stmt->parent, ancestor);
					apply_scope(prog, stmt->jump_to, target->parent, ancestor);
				}
			} else {
				if (stmt->jump_to > *prog->pc) {
					binding* b;
					target = GET_STMT(prog->stmt_list, stmt->parent);
					for (i=0; i<target->bindings->size; ++i) {
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

			*prog->pc = stmt->jump_to-1;
			break;

		case RETURN_STMT:
			if (prog->func->ret_val.type != VOID_TYPE)
				prog->func->ret_val.v.int_val = execute_expr(prog, stmt->exp);

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



int execute_expr(program_state* prog, expression* e)
{
	var_value* var;

	function* old_func, *func;
	vector_void* old_stmt_list;
	size_t* old_pc;


	switch (e->tok.type) {
	case EXP:          return execute_expr(prog, e->left);
	case ADD:          return execute_expr(prog, e->left) + execute_expr(prog, e->right);
	case SUB:          return execute_expr(prog, e->left) - execute_expr(prog, e->right);
	case MULT:         return execute_expr(prog, e->left) * execute_expr(prog, e->right);
	case DIV:          return execute_expr(prog, e->left) / execute_expr(prog, e->right);
	case MOD:          return execute_expr(prog, e->left) % execute_expr(prog, e->right);

	case GREATER:      return execute_expr(prog, e->left) > execute_expr(prog, e->right);
	case LESS:         return execute_expr(prog, e->left) < execute_expr(prog, e->right);
	case GTEQ:         return execute_expr(prog, e->left) >= execute_expr(prog, e->right);
	case LTEQ:         return execute_expr(prog, e->left) <= execute_expr(prog, e->right);
	case NOTEQUAL:     return execute_expr(prog, e->left) != execute_expr(prog, e->right);
	case EQUALEQUAL:   return execute_expr(prog, e->left) == execute_expr(prog, e->right);
		break;

	case COMMA:        return execute_expr(prog, e->left) , execute_expr(prog, e->right);

	case TERNARY:      return execute_expr(prog, e->left) ? execute_expr(prog, e->right->left) : execute_expr(prog, e->right->right);

	case EQUAL:
		var = look_up_value(prog, e->left->tok.v.id, BOTH);
		return var->v.int_val = execute_expr(prog, e->right);
	case ADDEQUAL:
		var = look_up_value(prog, e->left->tok.v.id, BOTH);
		return var->v.int_val += execute_expr(prog, e->right);
	case SUBEQUAL:
		var = look_up_value(prog, e->left->tok.v.id, BOTH);
		return var->v.int_val -= execute_expr(prog, e->right);
	case MULTEQUAL:
		var = look_up_value(prog, e->left->tok.v.id, BOTH);
		return var->v.int_val *= execute_expr(prog, e->right);
	case DIVEQUAL:
		var = look_up_value(prog, e->left->tok.v.id, BOTH);
		return var->v.int_val /= execute_expr(prog, e->right);
	case MODEQUAL:
		var = look_up_value(prog, e->left->tok.v.id, BOTH);
		return var->v.int_val %= execute_expr(prog, e->right);
	
		break;

	case LOGICAL_OR:    return execute_expr(prog, e->left) || execute_expr(prog, e->right);
	case LOGICAL_AND:   return execute_expr(prog, e->left) && execute_expr(prog, e->right);
	case LOGICAL_NEGATION: return !execute_expr(prog, e->left);


	case FUNC_CALL:
		old_stmt_list = prog->stmt_list;
		old_pc = prog->pc;
		old_func = prog->func;

		//look_up_value should never return NULL, parsing should catch all errors like that
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

		return func->ret_val.v.int_val;

	case ID:              return look_up_value(prog, e->tok.v.id, BOTH)->v.int_val;
	case INT_LITERAL:     return e->tok.v.integer;
	default:
		fprintf(stderr, "Interpreter error: Unrecognized op %d in expression.\n\n", e->tok.type);
		print_token(&e->tok);
		exit(0);
	}
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

	int i = 0;
	expression* tmp = e;
	while (e->tok.type == EXPR_LIST) {
		s = GET_SYMBOL(&func->symbols, i);
		v->val.v.int_val = execute_expr(prog, e->left);
		list_add(&v->list, &s->head);
		s->cur_parent = v->parent = 0; //reset for recursive call

		e = tmp = e->right;
		++i;
		v = malloc(sizeof(active_binding));
	}

	s = GET_SYMBOL(&func->symbols, i);
	v->val.v.int_val = execute_expr(prog, tmp);
	list_add(&v->list, &s->head);
	s->cur_parent = v->parent = 0;
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
	//puts("look_up_val");
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

	return NULL;
}



