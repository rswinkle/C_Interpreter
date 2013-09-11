#ifndef INTERPRETER_H
#define INTERPRETER_H


#include "parser.h"

#include "cvector.h"






void run(program_state* prog, char* start_func);
void execute(program_state* prog);
int execute_expr(program_state* prog, expression* exp);

var_value* look_up_value(program_state* prog, const char* var);
unsigned int look_up_loc(program_state* prog, const char* var);



#endif
