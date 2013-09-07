#ifndef INTERPRETER_H
#define INTERPRETER_H

#include "parser.h"


void run();
int execute(statement* stmt, int loc);
int execute_expr(statement* stmt);
int cond(statement* stmt);

int look_up_value(const char* var);
unsigned int look_up_loc(const char* var);



#endif
