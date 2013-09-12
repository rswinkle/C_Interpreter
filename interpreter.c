#include "interpreter.h"




void run(program_state* prog, char* start_func)
{
	while (prog->pc < prog->stmt_list.size) {
		execute(prog);
	}

	free_vec_void(&prog->stmt_list);
	free_vec_str(&prog->global_variables);
	free_vec_void(&prog->global_values);
}


void execute(program_state* prog)
{
	statement* stmt = GET_STMT(&prog->stmt_list, prog->pc);

	switch (stmt->type) {

	case PRINT_STMT:
		printf("%d\n", look_up_value(prog, stmt->lvalue)->v.int_val);
		break;

	case EXPR_STMT:
		execute_expr(prog, stmt->exp);
		break;

	case IF_STMT:
		if (!execute_expr(prog, stmt->exp))
			prog->pc = stmt->jump_to - 1;
		break;

	case WHILE_STMT:
		if (!execute_expr(prog, stmt->exp))
			prog->pc = stmt->jump_to - 1;
		break;

	case GOTO_STMT:
		prog->pc = stmt->jump_to-1;
		break;

	default:
		fprintf(stderr, "Interpreter error: unrecognized statement type.\n\n");
		exit(0);
	}

	prog->pc++;
}


int execute_expr(program_state* prog, expression* e)
{
	var_value* var;

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
		var = GET_VAR_VALUE(&prog->global_values, look_up_loc(prog, e->left->tok.v.id));
		return var->v.int_val = execute_expr(prog, e->right);
	case ADDEQUAL:
		var = GET_VAR_VALUE(&prog->global_values, look_up_loc(prog, e->left->tok.v.id));
		return var->v.int_val += execute_expr(prog, e->right);
	case SUBEQUAL:
		var = GET_VAR_VALUE(&prog->global_values, look_up_loc(prog, e->left->tok.v.id));
		return var->v.int_val -= execute_expr(prog, e->right);
	case MULTEQUAL:
		var = GET_VAR_VALUE(&prog->global_values, look_up_loc(prog, e->left->tok.v.id));
		return var->v.int_val *= execute_expr(prog, e->right);
	case DIVEQUAL:
		var = GET_VAR_VALUE(&prog->global_values, look_up_loc(prog, e->left->tok.v.id));
		return var->v.int_val /= execute_expr(prog, e->right);
	case MODEQUAL:
		var = GET_VAR_VALUE(&prog->global_values, look_up_loc(prog, e->left->tok.v.id));
		return var->v.int_val %= execute_expr(prog, e->right);
	
		break;

	case LOGICAL_OR:    return execute_expr(prog, e->left) || execute_expr(prog, e->right);
	case LOGICAL_AND:   return execute_expr(prog, e->left) && execute_expr(prog, e->right);


	case ID:              return look_up_value(prog, e->tok.v.id)->v.int_val;
	case INT_LITERAL:     return e->tok.v.integer;
	default:
		fprintf(stderr, "Interpreter error: Unrecognized op %d in expression.\n\n", e->tok.type);
		print_token(&e->tok);
		exit(0);
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

var_value* look_up_value(program_state* prog, const char* var)
{
	for (int i=0; i<prog->global_variables.size; ++i) {
		if (!strcmp(var, prog->global_variables.a[i]))
			return GET_VAR_VALUE(&prog->global_values, i);
	}

	fprintf(stderr, "Interpreter error: undeclared variable '%s'\n", var);

	exit(0);
}



