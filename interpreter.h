#ifndef INTERPRETER_H
#define INTERPRETER_H


#include "parser.h"

#include "cvector.h"






void run(program_state* prog, char* start_func);
void execute(program_state* prog);
int execute_expr(program_state* prog, unsigned int expr_loc);
void execute_expr_list(program_state* prog, function* callee, unsigned int expr_loc);


void add_variable(program_state* prog, char* var, var_type vtype);
void pop_scope(program_state* prog, vector_str* scope_vars);


variable* look_up_variable(program_state* prog, const char* var);
var_value* look_up_value(program_state* prog, const char* var, int search);
unsigned int look_up_loc(program_state* prog, const char* var);


#endif
