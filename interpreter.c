#include "interpreter.h"




void run(program_state* prog, char* start_func)
{
	while (prog->pc < prog->stmt_list.size) {
		execute(prog);
	}

	free_vec_void(&prog->stmt_list);
	free_vec_str(&prog->variables);
	free_vec_void(&prog->values);
}


void execute(program_state* prog)
{
	statement* stmt = GET_STMT(&prog->stmt_list, prog->pc);

	var_value* var;
	switch (stmt->type) {

	case PRINT_STMT:
		printf("%d\n", look_up_value(prog, stmt->lvalue)->v.int_val);
		break;

	case ASSIGN_STMT:
		GET_VAR_VALUE(&prog->values, look_up_loc(prog, stmt->lvalue))->v.int_val = execute_expr(prog, stmt->exp);
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


	case ID:              return look_up_value(prog, e->tok.v.id)->v.int_val;
	case INT_LITERAL:     return e->tok.v.integer;
	default:
		fprintf(stderr, "Interpreter error: Unrecognized op %d in expression.\n\n", e->tok.type);
		exit(0);
	}
}

unsigned int look_up_loc(program_state* prog, const char* var)
{
	for (int i=0; i<prog->variables.size; ++i) {
		if (!strcmp(var, prog->variables.a[i]))
			return i;
	}

	fprintf(stderr, "Interpreter error: undeclared variable '%s'\n", var);
	exit(0);
}

var_value* look_up_value(program_state* prog, const char* var)
{
	for (int i=0; i<prog->variables.size; ++i) {
		if (!strcmp(var, prog->variables.a[i]))
			return GET_VAR_VALUE(&prog->values, i);
	}

	fprintf(stderr, "Interpreter error: undeclared variable '%s'\n", var);

	exit(0);
}



