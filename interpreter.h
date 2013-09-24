#ifndef INTERPRETER_H
#define INTERPRETER_H


#include "parser.h"

#include "cvector.h"






void run(program_state* prog, char* start_func);
void execute(program_state* prog);
var_value execute_expr(program_state* prog, expression* e);
var_value execute_constant_expr(program_state* prog, expression* e);
void execute_expr_list(program_state* prog, function* callee, expression* e);
void execute_goto(program_state* prog, statement* stmt);

void execute_print(var_value a);
int is_true(var_value v);


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


/***********Operations ***************
 * Operator overloading is one of the few things 
 * I would add to C
typedef enum
{
	UNKNOWN = 0,

	CHAR_TYPE, 
	UCHAR_TYPE,

	SHORT_TYPE,
	USHORT_TYPE,

	INT_TYPE,
	SINT_TYPE,

	FLOAT_TYPE,
	DOUBLE_TYPE,

	FUNCTION_TYPE,
	VOID_TYPE,
	
	INT_PTR_TYPE
} var_type;

 */

#define PRE_OP(op, a) \
do { \
	switch (a->type) { \
	case SHORT_TYPE: \
		result.type = SHORT_TYPE; \
		result.v.short_val = op a->v.short_val; \
		break; \
	case USHORT_TYPE: \
		result.type = USHORT_TYPE; \
		result.v.ushort_val = op a->v.ushort_val; \
		break; \
	case INT_TYPE: \
		result.type = INT_TYPE; \
		result.v.int_val = op a->v.int_val; \
		break; \
	case UINT_TYPE: \
		result.type = UINT_TYPE; \
		result.v.uint_val = op a->v.uint_val; \
		break; \
	case LONG_TYPE: \
		result.type = LONG_TYPE; \
		result.v.long_val = op a->v.long_val; \
		break; \
	case ULONG_TYPE: \
		result.type = ULONG_TYPE; \
		result.v.ulong_val = op a->v.ulong_val; \
		break; \
	} \
} while (0)

#define POST_OP(a, op) \
do { \
	switch (a->type) { \
	case SHORT_TYPE: \
		result.type = SHORT_TYPE; \
		result.v.short_val = a->v.short_val op; \
		break; \
	case USHORT_TYPE: \
		result.type = USHORT_TYPE; \
		result.v.ushort_val = a->v.ushort_val op; \
		break; \
	case INT_TYPE: \
		result.type = INT_TYPE; \
		result.v.int_val = a->v.int_val op; \
		break; \
	case UINT_TYPE: \
		result.type = UINT_TYPE; \
		result.v.uint_val = a->v.uint_val op; \
		break; \
	case LONG_TYPE: \
		result.type = LONG_TYPE; \
		result.v.long_val = a->v.long_val op; \
		break; \
	case ULONG_TYPE: \
		result.type = ULONG_TYPE; \
		result.v.ulong_val = a->v.ulong_val op; \
		break; \
	} \
} while (0)


//parenthesis are necessary for comma expression since
//it has lower priority than assignment.  Before when I
//was returning ints directly it worked without because return evaluates
//the expression completely before returning ie it's lower priority
//
//Also note for now I'm assuming (true on my 64 bit linux machine)
//INT_MAX > USHORT_MAX
//LONG_MAX > UINT_MAX
//
//I should somehow put that check in and have correct conversion behavior
//either way
#define BINARY_OP(a, op, b) \
do { \
	switch (a->type) { \
	case SHORT_TYPE: \
		switch (b->type) { \
		case SHORT_TYPE: \
			result.type = SHORT_TYPE; \
			result.v.short_val = (a->v.short_val op b->v.short_val); \
			break; \
		\
		case USHORT_TYPE: \
			result.type = USHORT_TYPE; \
			result.v.ushort_val = (a->v.short_val op b->v.ushort_val); \
			break; \
		\
		case INT_TYPE: \
			result.type = INT_TYPE; \
			result.v.int_val = (a->v.short_val op b->v.int_val); \
			break; \
		\
		case UINT_TYPE: \
			result.type = UINT_TYPE; \
			result.v.uint_val = (a->v.short_val op b->v.uint_val); \
			break; \
		\
		case LONG_TYPE: \
			result.type = LONG_TYPE; \
			result.v.long_val = (a->v.short_val op b->v.long_val); \
			break; \
		\
		case ULONG_TYPE: \
			result.type = ULONG_TYPE; \
			result.v.ulong_val = (a->v.short_val op b->v.ulong_val); \
			break; \
		\
		case DOUBLE_TYPE: \
			result.type = DOUBLE_TYPE; \
			result.v.double_val = (a->v.short_val op b->v.double_val); \
			break; \
\
		default: \
			puts("wtf1"); \
		} \
		break; \
		\
	case USHORT_TYPE: \
		switch (b->type) { \
		case SHORT_TYPE: \
			result.type = USHORT_TYPE; \
			result.v.short_val = (a->v.ushort_val op b->v.short_val); \
			break; \
		\
		case USHORT_TYPE: \
			result.type = USHORT_TYPE; \
			result.v.ushort_val = (a->v.ushort_val op b->v.ushort_val); \
			break; \
		\
		case INT_TYPE: \
			result.type = INT_TYPE; \
			result.v.int_val = (a->v.ushort_val op b->v.int_val); \
			break; \
		\
		case UINT_TYPE: \
			result.type = UINT_TYPE; \
			result.v.uint_val = (a->v.ushort_val op b->v.uint_val); \
			break; \
		\
		case LONG_TYPE: \
			result.type = LONG_TYPE; \
			result.v.long_val = (a->v.ushort_val op b->v.long_val); \
			break; \
		\
		case ULONG_TYPE: \
			result.type = ULONG_TYPE; \
			result.v.ulong_val = (a->v.ushort_val op b->v.ulong_val); \
			break; \
		\
		case DOUBLE_TYPE: \
			result.type = DOUBLE_TYPE; \
			result.v.double_val = (a->v.ushort_val op b->v.double_val); \
			break; \
\
		default: \
			puts("wtf1"); \
		} \
		break; \
		\
	case INT_TYPE: \
		switch (b->type) { \
		case SHORT_TYPE: \
			result.type = INT_TYPE; \
			result.v.int_val = (a->v.int_val op b->v.short_val); \
			break; \
		\
		case USHORT_TYPE: \
			result.type = INT_TYPE; \
			result.v.int_val = (a->v.int_val op b->v.ushort_val); \
			break; \
		\
		case INT_TYPE: \
			result.type = INT_TYPE; \
			result.v.int_val = (a->v.int_val op b->v.int_val); \
			break; \
		\
		case UINT_TYPE: \
			result.type = UINT_TYPE; \
			result.v.uint_val = (a->v.int_val op b->v.uint_val); \
			break; \
		\
		case LONG_TYPE: \
			result.type = LONG_TYPE; \
			result.v.long_val = (a->v.int_val op b->v.long_val); \
			break; \
		\
		case ULONG_TYPE: \
			result.type = ULONG_TYPE; \
			result.v.ulong_val = (a->v.int_val op b->v.ulong_val); \
			break; \
		\
		case DOUBLE_TYPE: \
			result.type = DOUBLE_TYPE; \
			result.v.double_val = (a->v.int_val op b->v.double_val); \
			break; \
\
		default: \
			puts("wtf1"); \
		} \
		break; \
		\
	case UINT_TYPE: \
		switch (b->type) { \
		case SHORT_TYPE: \
			result.type = UINT_TYPE; \
			result.v.uint_val = (a->v.uint_val op b->v.short_val); \
			break; \
		\
		case USHORT_TYPE: \
			result.type = UINT_TYPE; \
			result.v.uint_val = (a->v.uint_val op b->v.ushort_val); \
			break; \
		\
		case INT_TYPE: \
			result.type = UINT_TYPE; \
			result.v.uint_val = (a->v.uint_val op b->v.int_val); \
			break; \
		\
		case UINT_TYPE: \
			result.type = UINT_TYPE; \
			result.v.uint_val = (a->v.uint_val op b->v.uint_val); \
			break; \
		\
		case LONG_TYPE: \
			result.type = LONG_TYPE; \
			result.v.long_val = (a->v.uint_val op b->v.long_val); \
			break; \
		\
		case ULONG_TYPE: \
			result.type = ULONG_TYPE; \
			result.v.ulong_val = (a->v.uint_val op b->v.ulong_val); \
			break; \
		\
		case DOUBLE_TYPE: \
			result.type = DOUBLE_TYPE; \
			result.v.double_val = (a->v.uint_val op b->v.double_val); \
			break; \
\
		default: \
			puts("wtf1"); \
		} \
		break; \
	\
	case LONG_TYPE: \
		switch (b->type) { \
		case SHORT_TYPE: \
			result.type = LONG_TYPE; \
			result.v.long_val = (a->v.long_val op b->v.short_val); \
			break; \
		\
		case USHORT_TYPE: \
			result.type = LONG_TYPE; \
			result.v.long_val = (a->v.long_val op b->v.ushort_val); \
			break; \
		\
		case INT_TYPE: \
			result.type = LONG_TYPE; \
			result.v.long_val = (a->v.long_val op b->v.int_val); \
			break; \
		\
		case UINT_TYPE: \
			result.type = LONG_TYPE; \
			result.v.long_val = (a->v.long_val op b->v.uint_val); \
			break; \
		\
		case LONG_TYPE: \
			result.type = LONG_TYPE; \
			result.v.long_val = (a->v.long_val op b->v.long_val); \
			break; \
		\
		case ULONG_TYPE: \
			result.type = ULONG_TYPE; \
			result.v.ulong_val = (a->v.long_val op b->v.ulong_val); \
			break; \
		\
		case DOUBLE_TYPE: \
			result.type = DOUBLE_TYPE; \
			result.v.double_val = (a->v.long_val op b->v.double_val); \
			break; \
\
		default: \
			puts("wtf1"); \
		} \
		break; \
		\
	case ULONG_TYPE: \
		switch (b->type) { \
		case SHORT_TYPE: \
			result.type = ULONG_TYPE; \
			result.v.ulong_val = (a->v.ulong_val op b->v.short_val); \
			break; \
		\
		case USHORT_TYPE: \
			result.type = ULONG_TYPE; \
			result.v.ulong_val = (a->v.ulong_val op b->v.ushort_val); \
			break; \
		\
		case INT_TYPE: \
			result.type = ULONG_TYPE; \
			result.v.ulong_val = (a->v.ulong_val op b->v.int_val); \
			break; \
		\
		case UINT_TYPE: \
			result.type = ULONG_TYPE; \
			result.v.ulong_val = (a->v.ulong_val op b->v.uint_val); \
			break; \
		\
		case LONG_TYPE: \
			result.type = ULONG_TYPE; \
			result.v.ulong_val = (a->v.ulong_val op b->v.long_val); \
			break; \
		\
		case ULONG_TYPE: \
			result.type = ULONG_TYPE; \
			result.v.ulong_val = (a->v.ulong_val op b->v.ulong_val); \
			break; \
		\
		case DOUBLE_TYPE: \
			result.type = DOUBLE_TYPE; \
			result.v.double_val = (a->v.ulong_val op b->v.double_val); \
			break; \
		} \
\
	case DOUBLE_TYPE: \
		switch (b->type) { \
		case SHORT_TYPE: \
			result.type = DOUBLE_TYPE; \
			result.v.double_val = (a->v.double_val op b->v.short_val); \
			break; \
		\
		case USHORT_TYPE: \
			result.type = DOUBLE_TYPE; \
			result.v.double_val = (a->v.double_val op b->v.ushort_val); \
			break; \
		\
		case INT_TYPE: \
			result.type = DOUBLE_TYPE; \
			result.v.double_val = (a->v.double_val op b->v.int_val); \
			break; \
		\
		case UINT_TYPE: \
			result.type = DOUBLE_TYPE; \
			result.v.double_val = (a->v.double_val op b->v.uint_val); \
			break; \
		\
		case LONG_TYPE: \
			result.type = DOUBLE_TYPE; \
			result.v.double_val = (a->v.double_val op b->v.long_val); \
			break; \
		\
		case ULONG_TYPE: \
			result.type = DOUBLE_TYPE; \
			result.v.double_val = (a->v.double_val op b->v.ulong_val); \
			break; \
		\
		case DOUBLE_TYPE: \
			result.type = DOUBLE_TYPE; \
			result.v.double_val = (a->v.double_val op b->v.double_val); \
			break; \
\
		default: \
			puts("wtf2"); \
		} \
		break; \
	default: \
		puts("wtf3"); \
 \
	} \
	\
} while (0)

#define INTEGRAL_BINARY_OP(a, op, b) \
do { \
	switch (a->type) { \
	case SHORT_TYPE: \
		switch (b->type) { \
		case SHORT_TYPE: \
			result.type = SHORT_TYPE; \
			result.v.short_val = (a->v.short_val op b->v.short_val); \
			break; \
		\
		case USHORT_TYPE: \
			result.type = USHORT_TYPE; \
			result.v.ushort_val = (a->v.short_val op b->v.ushort_val); \
			break; \
		\
		case INT_TYPE: \
			result.type = INT_TYPE; \
			result.v.int_val = (a->v.short_val op b->v.int_val); \
			break; \
		\
		case UINT_TYPE: \
			result.type = UINT_TYPE; \
			result.v.uint_val = (a->v.short_val op b->v.uint_val); \
			break; \
		\
		case LONG_TYPE: \
			result.type = LONG_TYPE; \
			result.v.long_val = (a->v.short_val op b->v.long_val); \
			break; \
		\
		case ULONG_TYPE: \
			result.type = ULONG_TYPE; \
			result.v.ulong_val = (a->v.short_val op b->v.ulong_val); \
			break; \
		\
		default: \
			puts("wtf1"); \
		} \
		break; \
		\
	case USHORT_TYPE: \
		switch (b->type) { \
		case SHORT_TYPE: \
			result.type = USHORT_TYPE; \
			result.v.short_val = (a->v.ushort_val op b->v.short_val); \
			break; \
		\
		case USHORT_TYPE: \
			result.type = USHORT_TYPE; \
			result.v.ushort_val = (a->v.ushort_val op b->v.ushort_val); \
			break; \
		\
		case INT_TYPE: \
			result.type = INT_TYPE; \
			result.v.int_val = (a->v.ushort_val op b->v.int_val); \
			break; \
		\
		case UINT_TYPE: \
			result.type = UINT_TYPE; \
			result.v.uint_val = (a->v.ushort_val op b->v.uint_val); \
			break; \
		\
		case LONG_TYPE: \
			result.type = LONG_TYPE; \
			result.v.long_val = (a->v.ushort_val op b->v.long_val); \
			break; \
		\
		case ULONG_TYPE: \
			result.type = ULONG_TYPE; \
			result.v.ulong_val = (a->v.ushort_val op b->v.ulong_val); \
			break; \
		\
		default: \
			puts("wtf1"); \
		} \
		break; \
		\
	case INT_TYPE: \
		switch (b->type) { \
		case SHORT_TYPE: \
			result.type = INT_TYPE; \
			result.v.int_val = (a->v.int_val op b->v.short_val); \
			break; \
		\
		case USHORT_TYPE: \
			result.type = INT_TYPE; \
			result.v.int_val = (a->v.int_val op b->v.ushort_val); \
			break; \
		\
		case INT_TYPE: \
			result.type = INT_TYPE; \
			result.v.int_val = (a->v.int_val op b->v.int_val); \
			break; \
		\
		case UINT_TYPE: \
			result.type = UINT_TYPE; \
			result.v.uint_val = (a->v.int_val op b->v.uint_val); \
			break; \
		\
		case LONG_TYPE: \
			result.type = LONG_TYPE; \
			result.v.long_val = (a->v.int_val op b->v.long_val); \
			break; \
		\
		case ULONG_TYPE: \
			result.type = ULONG_TYPE; \
			result.v.ulong_val = (a->v.int_val op b->v.ulong_val); \
			break; \
		\
		default: \
			puts("wtf1"); \
		} \
		break; \
		\
	case UINT_TYPE: \
		switch (b->type) { \
		case SHORT_TYPE: \
			result.type = UINT_TYPE; \
			result.v.uint_val = (a->v.uint_val op b->v.short_val); \
			break; \
		\
		case USHORT_TYPE: \
			result.type = UINT_TYPE; \
			result.v.uint_val = (a->v.uint_val op b->v.ushort_val); \
			break; \
		\
		case INT_TYPE: \
			result.type = UINT_TYPE; \
			result.v.uint_val = (a->v.uint_val op b->v.int_val); \
			break; \
		\
		case UINT_TYPE: \
			result.type = UINT_TYPE; \
			result.v.uint_val = (a->v.uint_val op b->v.uint_val); \
			break; \
		\
		case LONG_TYPE: \
			result.type = LONG_TYPE; \
			result.v.long_val = (a->v.uint_val op b->v.long_val); \
			break; \
		\
		case ULONG_TYPE: \
			result.type = ULONG_TYPE; \
			result.v.ulong_val = (a->v.uint_val op b->v.ulong_val); \
			break; \
		\
		default: \
			puts("wtf1"); \
		} \
		break; \
	\
	case LONG_TYPE: \
		switch (b->type) { \
		case SHORT_TYPE: \
			result.type = LONG_TYPE; \
			result.v.long_val = (a->v.long_val op b->v.short_val); \
			break; \
		\
		case USHORT_TYPE: \
			result.type = LONG_TYPE; \
			result.v.long_val = (a->v.long_val op b->v.ushort_val); \
			break; \
		\
		case INT_TYPE: \
			result.type = LONG_TYPE; \
			result.v.long_val = (a->v.long_val op b->v.int_val); \
			break; \
		\
		case UINT_TYPE: \
			result.type = LONG_TYPE; \
			result.v.long_val = (a->v.long_val op b->v.uint_val); \
			break; \
		\
		case LONG_TYPE: \
			result.type = LONG_TYPE; \
			result.v.long_val = (a->v.long_val op b->v.long_val); \
			break; \
		\
		case ULONG_TYPE: \
			result.type = ULONG_TYPE; \
			result.v.ulong_val = (a->v.long_val op b->v.ulong_val); \
			break; \
		\
		default: \
			puts("wtf1"); \
		} \
		break; \
		\
	case ULONG_TYPE: \
		switch (b->type) { \
		case SHORT_TYPE: \
			result.type = ULONG_TYPE; \
			result.v.ulong_val = (a->v.ulong_val op b->v.short_val); \
			break; \
		\
		case USHORT_TYPE: \
			result.type = ULONG_TYPE; \
			result.v.ulong_val = (a->v.ulong_val op b->v.ushort_val); \
			break; \
		\
		case INT_TYPE: \
			result.type = ULONG_TYPE; \
			result.v.ulong_val = (a->v.ulong_val op b->v.int_val); \
			break; \
		\
		case UINT_TYPE: \
			result.type = ULONG_TYPE; \
			result.v.ulong_val = (a->v.ulong_val op b->v.uint_val); \
			break; \
		\
		case LONG_TYPE: \
			result.type = ULONG_TYPE; \
			result.v.ulong_val = (a->v.ulong_val op b->v.long_val); \
			break; \
		\
		case ULONG_TYPE: \
			result.type = ULONG_TYPE; \
			result.v.ulong_val = (a->v.ulong_val op b->v.ulong_val); \
			break; \
		} \
\
	default: \
		puts("wtf3"); \
	} \
 \
} while (0)

#define COND_EXPR(cond, a, b) \
do { \
	var_value c = execute_expr(prog, cond); \
	switch (c.type) { \
	case SHORT_TYPE: \
		result = (c.v.short_val) ? *a : *b; \
		break; \
	case USHORT_TYPE: \
		result = (c.v.ushort_val) ? *a : *b; \
		break; \
	case INT_TYPE: \
		result = (c.v.int_val) ? *a : *b; \
		break; \
	case UINT_TYPE: \
		result = (c.v.uint_val) ? *a : *b; \
		break; \
	case LONG_TYPE: \
		result = (c.v.long_val) ? *a : *b; \
		break; \
	case ULONG_TYPE: \
		result = (c.v.ulong_val) ? *a : *b; \
		break; \
	case DOUBLE_TYPE: \
		result = (c.v.double_val) ? *a : *b; \
		break; \
	} \
} while (0)


#endif
