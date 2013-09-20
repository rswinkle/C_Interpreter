#ifndef INTERPRETER_H
#define INTERPRETER_H


#include "parser.h"

#include "cvector.h"






void run(program_state* prog, char* start_func);
void execute(program_state* prog);
int execute_expr(program_state* prog, expression* e);
int execute_constant_expr(program_state* prog, expression* e);
void execute_expr_list(program_state* prog, function* callee, expression* e);
void execute_goto(program_state* prog, statement* stmt);


void add_binding(program_state* prog, char* name, var_type vtype);
void remove_binding(program_state* prog, char* name);
void clear_bindings(program_state* prog);
void pop_scope(program_state* prog);
int is_ancestor(program_state* prog, int parent, int child);
void apply_scope(program_state* prog, int jump_to, int child, int parent);
void remove_scope(program_state* prog, int jump_to, int child, int parent);
int find_lowest_common_ancestor(program_state* prog, int parent1, int parent2);



symbol* look_up_symbol(program_state* prog, const char* var);
var_value* look_up_value(program_state* prog, const char* var, int search);
unsigned int look_up_loc(program_state* prog, const char* var);


#endif
