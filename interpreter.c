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

	//set parameters here, ie argc, argv
		
	execute(prog);

	var_value ret = prog->func->ret_val;
	
	free_vec_void(&prog->functions);
	free_vec_str(&prog->global_variables);
	free_vec_void(&prog->global_values);
	free_vec_void(&prog->expressions);
}




//TODO change int to var_value
void execute(program_state* prog)
{
	vector_str scope_variables;
	vec_str(&scope_variables, 0, 20);

	while ( *prog->pc < prog->stmt_list->size ) {
		statement* stmt = GET_STMT(prog->stmt_list, *prog->pc);

		switch (stmt->type) {

		case DECL_STMT:
			add_variable(prog, stmt->lvalue, stmt->vtype);
			push_str(&scope_variables, stmt->lvalue);
			break;

		case PRINT_STMT:
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

		case GOTO_STMT:
			*prog->pc = stmt->jump_to-1;
			break;

		case RETURN_STMT:
			if (prog->func->ret_val.type != VOID)
				prog->func->ret_val.v.int_val = execute_expr(prog, stmt->exp);

			goto exit_clean;

		case START_COMPOUND_STMT:
			(*prog->pc)++;
			execute(prog);
			stmt = GET_STMT(prog->stmt_list, *prog->pc);
			if (stmt->type != END_COMPOUND_STMT)
				goto exit_clean;
			break;
			
		case END_COMPOUND_STMT:
			goto exit_clean;
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

exit_clean:
	pop_scope(prog, &scope_variables);
	free_vec_str(&scope_variables);
	

}


int execute_expr(program_state* prog, unsigned int expr_loc)
{
	var_value* var;

	function* old_func, *func;
	vector_void* old_stmt_list;
	size_t* old_pc;

	expression* e = GET_EXPRESSION(&prog->expressions, expr_loc);
	expression* left = GET_EXPRESSION(&prog->expressions, e->left);
	expression* right = GET_EXPRESSION(&prog->expressions, e->right);

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

	case EQUAL:
		var = look_up_value(prog, left->tok.v.id, BOTH);
		return var->v.int_val = execute_expr(prog, e->right);
	case ADDEQUAL:
		var = look_up_value(prog, left->tok.v.id, BOTH);
		return var->v.int_val += execute_expr(prog, e->right);
	case SUBEQUAL:
		var = look_up_value(prog, left->tok.v.id, BOTH);
		return var->v.int_val -= execute_expr(prog, e->right);
	case MULTEQUAL:
		var = look_up_value(prog, left->tok.v.id, BOTH);
		return var->v.int_val *= execute_expr(prog, e->right);
	case DIVEQUAL:
		var = look_up_value(prog, left->tok.v.id, BOTH);
		return var->v.int_val /= execute_expr(prog, e->right);
	case MODEQUAL:
		var = look_up_value(prog, left->tok.v.id, BOTH);
		return var->v.int_val %= execute_expr(prog, e->right);
	
		break;

	case LOGICAL_OR:    return execute_expr(prog, e->left) || execute_expr(prog, e->right);
	case LOGICAL_AND:   return execute_expr(prog, e->left) && execute_expr(prog, e->right);


	case FUNC_CALL:
		old_stmt_list = prog->stmt_list;
		old_pc = prog->pc;
		old_func = prog->func;

		//look_up_value should never return NULL, parsing should catch all errors like that
		func = GET_FUNCTION(&prog->functions, look_up_value(prog, left->tok.v.id, ONLY_GLOBAL)->v.func_loc);

		if (func->n_params) //could also check e->right->tok.type = VOID
			execute_expr_list(prog, func, e->right);

		prog->func = func;
		prog->stmt_list = &func->stmt_list;
		func->pc = 0;
		prog->pc = &func->pc;

		execute(prog); //run program

		variable* param;
		for (int i=0; i<func->n_params; ++i) {
			param = GET_VARIABLE(&func->variables, i);
			var = list_entry(param->head.next, var_value, list);
			list_del(&var->list);
			free(var);
		}

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
void execute_expr_list(program_state* prog, function* callee, unsigned int expr_loc)
{
	function* func = callee;
	expression* e = GET_EXPRESSION(&prog->expressions, expr_loc);
	variable* var;
	var_value* v = malloc(sizeof(var_value));;

	int i = 0, tmp = -1;
	while (e->tok.type == EXPR_LIST) {
		var = GET_VARIABLE(&func->variables, i);
		//v = list_entry(var->head.next, var_value, list);
		v->v.int_val = execute_expr(prog, e->left);
		list_add(&v->list, &var->head);

		tmp = e->right;
		e = GET_EXPRESSION(&prog->expressions, e->right);
		++i;
		v = malloc(sizeof(var_value));
	}

	var = GET_VARIABLE(&func->variables, i);
	//v = list_entry(var->head.next, var_value, list);
	v->v.int_val = execute_expr(prog, (tmp < 0) ? expr_loc : tmp);
	list_add(&v->list, &var->head);
}	

void add_variable(program_state* prog, char* name, var_type vtype)
{
	var_value* value = malloc(sizeof(var_value)); assert(value);
	value->type = vtype;

	variable new_var;
	variable* exists = look_up_variable(prog, name);
	if (!exists) {
		new_var.name = mystrdup(name);
		push_void(&prog->func->variables, &new_var);

		variable* tmp = back_void(&prog->func->variables);
		tmp->cur_scope = 0;
		INIT_LIST_HEAD(&tmp->head);
		list_add(&value->list, &tmp->head);
		
		//order of op, maybe separate
	} else {
		//since lookup always returns first item in list (top of binding stack)
		//head is ->prev
		list_add(&value->list, &exists->head);
		exists->cur_scope++;
		
	}
}

void pop_scope(program_state* prog, vector_str* scope_vars)
{
	var_value* val;
	variable* var;
	for (int i=0; i<scope_vars->size; ++i) {
		var = look_up_variable(prog, scope_vars->a[i]);
		val = list_entry(var->head.next, var_value, list);
		list_del(&val->list);
		free(val);

		var->cur_scope--;

		//TODO erase variable if empty maybe
		//if (list_empty( ))
		//	erase_void(&prog->func->variables, )
	}
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

variable* look_up_variable(program_state* prog, const char* var)
{
	variable* v;
	for (int i=0; i<prog->func->variables.size; ++i) {
		v = GET_VARIABLE(&prog->func->variables, i);
		if (!strcmp(var, v->name))
			return v;
	}
	return NULL;
}


var_value* look_up_value(program_state* prog, const char* var, int search)
{
	//puts("look_up_val");
	variable* v;
	if (prog->func && search != ONLY_GLOBAL) {
		for (int i=0; i<prog->func->variables.size; ++i) {
			v = GET_VARIABLE(&prog->func->variables, i);
	//		printf("checking '%s'\n", v->name);
			if (!strcmp(var, v->name)) {
				if (!list_empty(&v->head))
					//->next is first item, top of binding stack
					return list_entry(v->head.next, var_value, list);
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



