#ifndef PREPROCESSOR_H
#define PREPROCESSOR_H

#include "rsw_cstr.h"

//brings in lexer.h
#include "cvector_token_lex.h"

#define CVEC_STRDUP mystrdup
#include "cvector.h"


#include <stdio.h>
#include <stdarg.h>

#define GET_PARAM(VEC, I) CVEC_GET_VOID(VEC, macro_params, I)

typedef struct macro_params
{
	int num_params;
	char** names;
} macro_params;

/*
 * cvector_void or generated of this?
typedef struct macro
{
	char* name;
	char* value;
	macro_params params;
} macro;
*/

typedef struct preprocessor_state
{
	cvector_str macros;
	cvector_str values;
	
	cvector_void params;

	cvector_token_lex if_stack;

	lexer_state lexer;
	FILE* input;
	FILE* output;
} preprocessor_state;



void free_macro_params(void* param);

FILE* run_preprocessor(FILE* input, char* filename);

void preprocess_file(preprocessor_state* preproc);


void handle_define(preprocessor_state* preproc);
void handle_include(preprocessor_state* preproc);
void handle_ifdef(preprocessor_state* preproc);

void parse_params(preprocessor_state* preproc, int macro, rsw_cstr* expansion);
unsigned int macro_expansion(preprocessor_state* preproc, rsw_cstr* expansion, unsigned long beginning, cvector_i* valid_macros, int macro_index);
void prescan_argument(preprocessor_state* preproc, rsw_cstr* expansion);
void rescan_expansion(preprocessor_state* preproc, rsw_cstr* expansion, cvector_i* valid_macros, int macro_index);
void handle_macro(preprocessor_state* preproc, int macro);



int look_up_macro_loc(preprocessor_state* proc, char* macro);
char* look_up_macro(preprocessor_state* proc, char* macro);


void preprocessor_error(token_lex* tok, lexer_state* lexer, char* str, ...);





#endif
