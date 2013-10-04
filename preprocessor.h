#ifndef PREPROCESSOR_H
#define PREPROCESSOR_H

#include "vector_char.h"

#include "cvector.h"

#include "lexer.h"

#include <stdio.h>
#include <stdarg.h>

#define GET_PARAM(VEC, I) GET_VOID(VEC, macro_params, I)

typedef struct macro_params
{
	int num_params;
	char** names;
} macro_params;

/*
 * vector_void or generated of this?
typedef struct macro
{
	char* name;
	char* value;
	macro_params params;
} macro;
*/

typedef struct preprocessor_state
{
	vector_str macros;
	vector_str values;
	
	vector_void params;

} preprocessor_state;



void free_macro_params(void* param);

FILE* run_preprocessor(FILE* input);

void preprocess_file(FILE* input, preprocessor_state* preproc, FILE* output, lexer_state* lexer);


void handle_define(FILE* input, preprocessor_state* preproc, lexer_state* lexer);
void handle_include(FILE* input, preprocessor_state* preproc, FILE* output, lexer_state* lexer);

void parse_params(FILE* input, preprocessor_state* preproc, lexer_state* lexer, int macro, vector_char* expansion);
unsigned int macro_expansion(preprocessor_state* preproc, vector_char* expansion, unsigned long beginning, vector_i* valid_macros, int macro_index);
void prescan_argument(preprocessor_state* preproc, vector_char* expansion);
void rescan_expansion(preprocessor_state* preproc, vector_char* expansion, vector_i* valid_macros, int macro_index);
void handle_macro(FILE* input, preprocessor_state* preproc, lexer_state* lexer, FILE* output, int macro);



int look_up_macro_loc(preprocessor_state* proc, char* macro);
char* look_up_macro(preprocessor_state* proc, char* macro);


void preprocessor_error(token_value* tok, char* str, ...);





#endif
