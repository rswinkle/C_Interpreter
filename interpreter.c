#include "interpreter.h"


extern vector_void stmt_list;
extern vector_str variables;
extern vector_i values;


void run()
{
	int loc = 0;

	while (loc < stmt_list.size) {
		loc = execute(GET_STMT(&stmt_list, loc), loc);
	}

	free_vec_str(&variables);
	free_vec_i(&values);
}


int execute(statement* stmt, int loc)
{
	switch (stmt->type) {

	case PRINT_STMT:
		printf("%d\n", look_up_value(stmt->lvalue));
		break;

	case ASSIGN_STMT:
		values.a[look_up_loc(stmt->lvalue)] = execute_expr(stmt);
		break;

	case IF_STMT:
		if (!cond(stmt))
			loc = stmt->jump_to - 1;
		break;

	case WHILE_STMT:
		if (!cond(stmt))
			loc = stmt->jump_to - 1;
		break;

	case GOTO_STMT:
		loc = stmt->jump_to-1;
		break;

	default:
		fprintf(stderr, "Interpreter error: unrecognized statement type.\n\n");
		exit(0);
	}

	return ++loc;
}


int execute_expr(statement* stmt)
{
	int l, r;
	l = (stmt->left.type == ID) ? look_up_value(stmt->left.id) : stmt->left.num;
	if (stmt->op == SEMICOLON)
		return l;

	r = (stmt->right.type == ID) ? look_up_value(stmt->right.id) : stmt->right.num;

	switch (stmt->op) {
	case PLUS:		return l + r;
	case MINUS:		return l - r;
	case MULT:		return l * r;
	case DIV:		return l / r;
	default:
		fprintf(stderr, "Interpreter error: Unrecognized op in expression.\n\n");
		exit(0);
	}
}

unsigned int look_up_loc(const char* var)
{
	for (int i=0; i<variables.size; ++i) {
		if (!strcmp(var, variables.a[i]))
			return i;
	}

	fprintf(stderr, "Interpreter error: undeclared variable '%s'\n", var);
	exit(0);
}

int look_up_value(const char* var)
{
	for (int i=0; i<variables.size; ++i) {
		if (!strcmp(var, variables.a[i]))
			return values.a[i];
	}

	fprintf(stderr, "Interpreter error: undeclared variable '%s'\n", var);
	exit(0);
}

int cond(statement* stmt)
{
	int l, r;

	l = (stmt->left.type == ID) ? look_up_value(stmt->left.id) : stmt->left.num;
	r = (stmt->right.type == ID) ? look_up_value(stmt->right.id) : stmt->right.num;

	switch (stmt->op) {
	case GREATER:	return l > r;
	case LESS:		return l < r;
	case GTEQ:		return l >= r;
	case LTEQ:		return l <= r;
	case NOTEQUAL:	return l != r;
	default:
		fprintf(stderr, "Interpreter error: Unrecognized relop in condition.\n\n");
		exit(0);
	}
}




