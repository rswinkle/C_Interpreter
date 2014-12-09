
#include "preprocessor.h"

#define VECTOR_char_IMPLEMENTATION
#include "vector_char.h"


#include "c_utils.h"

#include <ctype.h>


#define MAX_MACRO_LEN 4096
static char macro_buf[MAX_MACRO_LEN];

void free_macro_params(void* param)
{
	macro_params* p = param;
	for (int i=0; i<p->num_params; ++i)
		free(p->names[i]);
	free(p->names);
}


FILE* run_preprocessor(FILE* input, char* filename)
{
	assert(input);
	
	preprocessor_state preproc;

	preproc.input = input;

	preproc.output = tmpfile();
	assert(preproc.output);

	vec_str(&preproc.macros, 0, 15);
	vec_str(&preproc.values, 0, 15);
	vec_void(&preproc.params, 0, 15, sizeof(macro_params), free_macro_params, NULL);
	vec_token_lex(&preproc.if_stack, 0, 10, free_token_lex, NULL);
	
	preproc.lexer = (lexer_state){ filename, 1, 0, 0 }; //TODO
	preprocess_file(&preproc);

	free_vec_str(&preproc.macros);
	free_vec_str(&preproc.values);
	free_vec_void(&preproc.params);
	free_vec_token_lex(&preproc.if_stack);
	fclose(input);

	rewind(preproc.output);
	return preproc.output;
}

void preprocess_file(preprocessor_state* preproc)
{
	lexer_state save_lexer, *lexer = &preproc->lexer;
	long fpos;
	int c;

	token_lex tlex[10]; 

	FILE* input = preproc->input;
	FILE* output = preproc->output;

	// if not -P
	fprintf(output, "# 0 \"%s\"\n", lexer->cur_file);

	tlex[0] = read_token(input, lexer, output);
	for ( ; tlex[0].tok.type != END; tlex[0] = read_token(input, lexer, output)) {

		if (tlex[0].tok.type == ID) {
			int is_macro = look_up_macro_loc(preproc, tlex[0].tok.v.id);
			if (is_macro >= 0) {
				macro_params* p = GET_PARAM(&preproc->params, is_macro);
				//for a function style macro if the name isn't followed by a ( it's not an error
				//it's just not an invocation so just print it out
				if (p->num_params >= 0) {
					save_lexer = *lexer;
					if ((fpos = ftell(input)) == -1) {
						perror("ftell failure in preprocess_file");
						exit(0);
					}
					tlex[1] = read_token(input, lexer, NULL);
					if (tlex[1].tok.type != LPAREN) {
						*lexer = save_lexer;
						fseek(input, fpos, SEEK_SET);

						if (tlex[1].tok.type == ID || tlex[1].tok.type == STR_LITERAL)
							free(tlex[1].tok.v.id);

						print_token(&tlex[0].tok, output, 0);
						free(tlex[0].tok.v.id);
						continue;
					}
					//restore ( for symmetry in handle_macro
					*lexer = save_lexer;
					fseek(input, fpos, SEEK_SET);
				}

				handle_macro(preproc, is_macro);
			} else {
				print_token(&tlex[0].tok, output, 0);
			}
			free(tlex[0].tok.v.id);

		} else if (tlex[0].tok.type == POUND) {
			//has to be the first token
			if (lexer->cur_tok != 1) {
				preprocessor_error(NULL, lexer, "Error: stray # in program\n");
			}

			save_lexer = *lexer;
			if ((fpos = ftell(input)) == -1) {
				perror("ftell failure in preprocess_file");
				exit(0);
			}
			tlex[1] = read_token(input, lexer, NULL);
			if (tlex[0].line != tlex[1].line) { //null directive (EOF is always on it's own line)
				if (tlex[1].tok.type == POUND) {
					tlex[0] = tlex[1];
					continue;
				} else if (tlex[1].tok.type == ID || tlex[1].tok.type == STR_LITERAL) {
					free(tlex[1].tok.v.id);
				}
				*lexer = save_lexer;
				if (fseek(input, fpos, SEEK_SET)) {
					perror("fseek failure in preprocess_file");
					exit(0);
				}
				continue;
			}

			
			if (tlex[1].tok.type != ID) {
				preprocessor_error(&tlex[1], lexer, "expected ID as macro name,");
			}
			
			if (!strcmp(tlex[1].tok.v.id, "define")) {
				free(tlex[1].tok.v.id);

				handle_define(preproc);
				goto eat_newline;
			}

			if (!strcmp(tlex[1].tok.v.id, "undef")) {
				free(tlex[1].tok.v.id);
				save_lexer = *lexer;
				fpos = ftell(input);
				tlex[1] = read_token(input, lexer, NULL);
				if (tlex[1].line != tlex[0].line) {
					preprocessor_error(NULL, lexer, "macro name missing in undef directive\n");
				}

				if (tlex[1].tok.type != ID) {
					preprocessor_error(&tlex[1], lexer, "ID expected in undef directive,");
				}
			
				int exists = look_up_macro_loc(preproc, tlex[0].tok.v.id);
				if (exists >= 0) {
					erase_str(&preproc->macros, exists, exists);
					erase_str(&preproc->values, exists, exists);
					erase_void(&preproc->params, exists, exists);
				}
				free(tlex[1].tok.v.id);

				goto eat_newline;
			}

			if (!strcmp(tlex[1].tok.v.id, "include")) {
				free(tlex[1].tok.v.id);
				handle_include(preproc);
				goto eat_newline;
			}
			if (!strcmp(tlex[1].tok.v.id, "if")) {
				free(tlex[1].tok.v.id);
			}

			if (!strcmp(tlex[1].tok.v.id, "ifdef")) {
				free(tlex[1].tok.v.id);
				handle_ifdef(preproc);
				goto eat_newline;
			}

			if (!strcmp(tlex[1].tok.v.id, "ifndef")) {
				free(tlex[1].tok.v.id);
			}

			if (!strcmp(tlex[1].tok.v.id, "else")) {
			}
			if (!strcmp(tlex[1].tok.v.id, "endif")) {
			}
			if (!strcmp(tlex[1].tok.v.id, "elif")) {
			}

			if (!strcmp(tlex[1].tok.v.id, "defined")) {
			}
			if (!strcmp(tlex[1].tok.v.id, "pragma")) {
			}

			if (!strcmp(tlex[1].tok.v.id, "error")) {
				//check for string
				//preprocessor_error(NULL, 

			}

eat_newline:
			//the newline should reset the position
			//but since the line of a directive should be
			//invisible to the final program it shouldn't affect line count
			do {
				c = getc(preproc->input);
				if (!isspace(c))
					preprocessor_error(NULL, lexer, "extra tokens in directive\n");
			} while (c != '\n');
			lexer->cur_pos = 1;
			lexer->cur_tok = 0;
		}
	}
}
	

void handle_ifdef(preprocessor_state* preproc)
{
	token_lex tok_lex = read_token(preproc->input, &preproc->lexer, NULL);
	if (tok_lex.tok.type != ID)
		preprocessor_error(&tok_lex, &preproc->lexer, "expected ID following #ifdef,");




}

void handle_define(preprocessor_state* preproc)
{
	FILE* input = preproc->input;
	lexer_state* lexer = &preproc->lexer;

	vector_token_lex tlex;
	vec_token_lex(&tlex, 0, 10, free_token_lex, NULL);
	
	token_lex tok_lex[4];
	tok_lex[0] = read_token(input, lexer, NULL);

	if (tok_lex[0].tok.type != ID) {
		preprocessor_error(&tok_lex[0], lexer, "expected ID for macro name,");
	}
	
	macro_params p = { -1, NULL };
	char* check_val = look_up_macro(preproc, tok_lex[0].tok.v.id);

	lexer_state save_lex = *lexer;
	long fpos = ftell(input);
	tok_lex[1] = read_token(input, lexer, NULL);

	if (tok_lex[1].line != tok_lex[0].line) { //object macro empty string
		*lexer = save_lex;
		fseek(input, fpos, SEEK_SET);

		if (check_val && strcmp(check_val, "")) {
			preprocessor_error(NULL, lexer, "redefinition of %s\n", tok_lex[0].tok.v.id);
		}
		if (!check_val) {
			push_str(&preproc->macros, tok_lex[0].tok.v.id);
			push_str(&preproc->values, "");
			push_void(&preproc->params, &p);
		}
		goto exit;
	}
	
	if (tok_lex[1].tok.type == LPAREN) { //function style macro
		int n = 0, i;
		tok_lex[2] = read_token(input, lexer, NULL);
		while (tok_lex[2].tok.type == ID) {
			n++;
			push_token_lex(&tlex, &tok_lex[2]);
			//don't free id because vector will
			tok_lex[3] = read_token(input, lexer, NULL);
			if (tok_lex[3].tok.type != COMMA)
				break;

			tok_lex[2] = read_token(input, lexer, NULL);
		}

		//TODO better way to express error logic?
		if (tok_lex[2].tok.type != ID) {
 		   	if (n != 0 || n == 0 && tok_lex[2].tok.type != RPAREN)
				preprocessor_error(NULL, lexer, "improperly formed function style macro\n");
		} else if (tok_lex[3].tok.type != RPAREN) {
			preprocessor_error(NULL, lexer, "improperly formed function style macro\n");
		}

		p.num_params = n;
		if (n) {
			p.names = malloc(n * sizeof(char*)); assert(p.names);
		}

		for (i=0; i < n; ++i) {
			p.names[i] = mystrdup(tlex.a[i].tok.v.id);
		}
	} else {
		//put it back, it's part of the macro value
		//it's handled below
		*lexer = save_lex;
		fseek(input, fpos, SEEK_SET);
	}

	memset(macro_buf, 0, MAX_MACRO_LEN);

	save_lex = *lexer;
	if ((fpos = ftell(input)) == -1) {
		perror("ftell failure in handle_define");
		exit(0);
	}

	tok_lex[2] = read_token(input, lexer, NULL);

	while (tok_lex[0].line == lexer->cur_line) {
		push_token_lex(&tlex, &tok_lex[2]);
		save_lex = *lexer;
		if ((fpos = ftell(input)) == -1) {
			perror("ftell failure in handle_define");
			exit(0);
		}
		//don't free id bceause vector will do it
		tok_lex[2] = read_token(input, lexer, NULL);
	}

	if (tok_lex[2].tok.type == ID || tok_lex[2].tok.type == STR_LITERAL)
		free(tok_lex[2].tok.v.id);

	*lexer = save_lex;
	if (fseek(input, fpos, SEEK_SET)) {
		perror("fseek failue in handle_define");
		exit(0);
	}

	int buf_pos = 0;
	int res, len;

	for (int i= (p.num_params == -1) ? 0 : p.num_params; i < tlex.size; ++i) {
		res = print_token_to_str(&tlex.a[i].tok, &macro_buf[buf_pos], MAX_MACRO_LEN-buf_pos);

		if (res < 0 || res >= MAX_MACRO_LEN - buf_pos) {
			preprocessor_error(NULL, lexer, "macro is too long or output error\n");
		}
		len = res;
		buf_pos += res;

		//	inserting a space for each whitespace character
		//	ie it wouldn't look the same if the programmer used any tabs etc but it's good enough
		if (i != tlex.size-1 && tlex.a[i+1].pos > tlex.a[i].pos + len) {
			res = snprintf(&macro_buf[buf_pos], MAX_MACRO_LEN-buf_pos, "%*s", tlex.a[i+1].pos - (tlex.a[i].pos + len), ""); 

			if (res < 0 || res >= MAX_MACRO_LEN - buf_pos) {
				preprocessor_error(NULL, lexer, "macro is too long or output error\n");
			}
			buf_pos += res;
		}
	}

	//benign redefinitions are allowed (ie the definitions are the same)
	if (check_val && strcmp(check_val, macro_buf)) {
		preprocessor_error(NULL, lexer, "redefinition of macro %s\n", tlex.a[0].tok.v.id);
	}

	//push new macro
	push_str(&preproc->macros, tok_lex[0].tok.v.id);
	push_str(&preproc->values, macro_buf);
	push_void(&preproc->params, &p);

exit:
	free(tok_lex[0].tok.v.id);  // name of macro

	if (tok_lex[1].tok.type == ID || tok_lex[1].tok.type == STR_LITERAL)
		free(tok_lex[1].tok.v.id);


	free_vec_token_lex(&tlex);
}




void handle_include(preprocessor_state* preproc)
{
	int line = preproc->lexer.cur_line;
	token_lex tok_lex = read_token(preproc->input, &preproc->lexer, NULL);
	if (tok_lex.line != line)
		preprocessor_error(NULL, &preproc->lexer, "include directive arguments should all be on the same line\n");
	
	if (tok_lex.tok.type != LESS && tok_lex.tok.type != STR_LITERAL) {
		preprocessor_error(&tok_lex, &preproc->lexer, "expected < or \" for include,");
	}
	
	lexer_state save_lexer;
	long fpos;
	//I don't support <> for now
	if (tok_lex.tok.type == LESS) {
		//just clear the line
		while (1) {
			save_lexer = preproc->lexer;
			if ((fpos = ftell(preproc->input)) == -1) {
				perror("ftell error in handle_include");
				exit(0);
			}
			tok_lex = read_token(preproc->input, &preproc->lexer, NULL);
			if (tok_lex.line != line) {
				if (tok_lex.tok.type == ID || tok_lex.tok.type == STR_LITERAL)
					free(tok_lex.tok.v.id);
				preproc->lexer = save_lexer;
				if (fseek(preproc->input, fpos, SEEK_SET)) {
					perror("fseek failure in handle_include");
					exit(0);
				}
				return;
			}
			if (tok_lex.tok.type == ID || tok_lex.tok.type == STR_LITERAL)
				free(tok_lex.tok.v.id);
		}
	}

	FILE* file = fopen(tok_lex.tok.v.id, "r");
	if (!file) {
		preprocessor_error(NULL, &preproc->lexer, "error opening file %s\n", tok_lex.tok.v.id);
	}

	save_lexer = preproc->lexer;
	FILE* save_input = preproc->input;
	preproc->lexer = (lexer_state){ tok_lex.tok.v.id, 1, 0, 0 };
	preproc->input = file;
	
	preprocess_file(preproc);

	preproc->lexer = save_lexer;
	preproc->input = save_input;

	fprintf(preproc->output, "# %u \"%s\"\n", save_lexer.cur_line, save_lexer.cur_file);

	free(tok_lex.tok.v.id);
	fclose(file);
}


void parse_params(preprocessor_state* preproc, int macro, vector_char* expansion)
{
	macro_params* p = GET_PARAM(&preproc->params, macro);
	token_lex t;

	vector_char scratch;
	vec_char_copy(&scratch, expansion);
//	init_vec_char(&scratch, expansion->a, expansion->size);

	//read (
	read_token(preproc->input, &preproc->lexer, NULL);

	int param_len, val_len, parens, loc;
	char* search, *found;

	vector_char arg_expansion;
	vec_char(&arg_expansion, 0, 50);

	parens = 0;
	for (int i=0; i < p->num_params; ++i) {
		val_len = 0;

		t = read_token(preproc->input, &preproc->lexer, NULL);
		if (t.tok.type == LPAREN)
			parens++;
		do {
			val_len += print_token_to_str(&t.tok, macro_buf+val_len, MAX_MACRO_LEN-val_len);

			if (t.tok.type == ID || t.tok.type == STR_LITERAL)
				free(t.tok.v.id);

			t = read_token(preproc->input, &preproc->lexer, NULL);
			if (t.tok.type == LPAREN) {
				parens++;
			} else if (t.tok.type == RPAREN) {
				parens--;
				if (parens == -1) {
					if (i != p->num_params-1)
						preprocessor_error(&t, &preproc->lexer, "more arguments expected for macro,");
					break;
				}
			}
		} while (t.tok.type != COMMA || parens > 0); //could put && parens != -1 and change above

		//perform prescan to handle nested calls
		//This contradicts C : A Reference Manual pg 50 but it's the only/easiest
		//way to get correct behavior (ie preventing infinite recursion but allowing nested calls to the same macro)
		//I know gcc does this (http://gcc.gnu.org/onlinedocs/gcc-3.0.2/cpp_3.html#SEC32)
		insert_array_char(&arg_expansion, 0, macro_buf, val_len+1); //+1 for '\0'
		prescan_argument(preproc, &arg_expansion);

		param_len = strlen(p->names[i]);
	//	val_len = strlen(macro_buf);
		search = scratch.a;  //search scratch
		
		found = strstr(search, p->names[i]);
		while (found) {
			loc = found - scratch.a;
			erase_char(expansion, loc, loc + param_len - 1);
			erase_char(&scratch, loc, loc + param_len - 1);
			insert_array_char(expansion, loc, arg_expansion.a, arg_expansion.size-1);
			insert_array_char(&scratch, loc, arg_expansion.a, arg_expansion.size-1);

			//overwrite replacement in scratch with a character not allowed in identifiers
			//so that replacement of a subsequent parameter won't replace within an expansion
			//ie #define MYMACRO(a, b) a + b;
			//called as MYMACRO(b, a) would wrongly go to b + b -> a + a instead of b + a
			for (int j=loc; j < loc+arg_expansion.size-1; ++j)
				scratch.a[j] = '*';

			search = &scratch.a[loc + arg_expansion.size-1];
			found = strstr(search, p->names[i]);
		}


		//reset for next argument
		arg_expansion.size = 0;
	}

	//read ) for 0 params, others get read in loop
	if (!p->num_params) {
		token_lex t = read_token(preproc->input, &preproc->lexer, NULL);
		if (t.tok.type != RPAREN)
			preprocessor_error(&t, &preproc->lexer, "expected ) in function style macro with 0 arguments,");
	}

	free_vec_char(&arg_expansion);
	free_vec_char(&scratch);
}

unsigned int macro_expansion(preprocessor_state* preproc, vector_char* expansion, unsigned long beginning, vector_i* valid_macros, int macro_index)
{
	//vaild_macros should never be NULL unless I uncomment that alternative method in prescan_argument
	//int macro = (valid_macros) ? valid_macros->a[macro_index] : macro_index;
	int macro = valid_macros->a[macro_index];

	vector_char local_expansion, scratch;
	init_vec_char(&local_expansion, preproc->values.a[macro], strlen(preproc->values.a[macro])+1); //+ 1 for '\0'
	vec_char_copy(&scratch, &local_expansion);

	vector_char arg_expansion;
	vec_char(&arg_expansion, 0, 50);

	macro_params* p = GET_PARAM(&preproc->params, macro);
	int param_len, val_len, parens, loc;

	char* search, *found, *c;

	//beginning is start of macro name so jump forward len (the first c++ will jump over '(' )
	c = &expansion->a[beginning + strlen(preproc->macros.a[macro])];
	if (p->num_params != -1)
		while (isspace(*c)) ++c;

	parens = 0;
	for (int i=0; i < p->num_params; ++i) {
		val_len = 0;

		c++;
		while (isspace(*c)) ++c; //ignore spaces on edges ie after ( and ,

		if (*c == '(')
			parens++;
		do {
			val_len += snprintf(macro_buf+val_len, MAX_MACRO_LEN-val_len, "%c", *c);

			c++;
			if (*c == '(') {
				parens++;
			} else if (*c == ')') {
				parens--;
				if (parens == -1) {
					if (i != p->num_params-1)
						preprocessor_error(NULL, &preproc->lexer, "more arguments expected for macro\n");
					break;
				}
			}
		} while (*c != ',' || parens > 0);

		insert_array_char(&arg_expansion, 0, macro_buf, val_len+1); //+1 for '\0'
		prescan_argument(preproc, &arg_expansion);

		param_len = strlen(p->names[i]);
		search = scratch.a;
		
		found = strstr(search, p->names[i]);
		while (found) {
			loc = found - scratch.a;
			erase_char(&local_expansion, loc, loc + param_len - 1);
			erase_char(&scratch, loc, loc + param_len - 1);
			insert_array_char(&local_expansion, loc, arg_expansion.a, arg_expansion.size-1);
			insert_array_char(&scratch, loc, arg_expansion.a, arg_expansion.size-1);

			for (int j=loc; j < loc+arg_expansion.size-1; ++j)
				scratch.a[j] = '*';

			search = &scratch.a[loc + arg_expansion.size-1];
			found = strstr(search, p->names[i]);
		}

		arg_expansion.size = 0;
	}
	
	if (!p->num_params) // increment for ) because it didn't get it in loop
		++c;

	//TODO test without this
	if (p->num_params == -1)
		--c;

	rescan_expansion(preproc, &local_expansion, valid_macros, macro_index);

	erase_char(expansion, beginning, c - expansion->a);
	insert_array_char(expansion, beginning, local_expansion.a, local_expansion.size-1);

	unsigned int local_size = local_expansion.size;

	free_vec_char(&local_expansion);
	free_vec_char(&scratch);
	free_vec_char(&arg_expansion);

	return beginning + local_size - 1;
}


void prescan_argument(preprocessor_state* preproc, vector_char* expansion)
{
	int loc, macro_name_len, macro_val_len;
	char* ptr, *search, *found;
	macro_params* p;

	/*
 	 * alternative method, actually parse tokens from the string
 	 * seems like overkill for now but if I run into some issue that makes it
 	 * more reasonable I'll consider it
 	 *
	vector_token_lex tlex;
	vec_token_lex(&tlex, 0, 20, free_token_lex, NULL);
	token_lex tok_lex;

	lexer_state lex = { 1, 0, 0, 0};
	tok_lex = read_token_from_str(&expansion->a[lex.cur_char], &lex, NULL);
	while (tok_lex.tok.type != END) {
		push_token_lex(&tlek, &tok_lex);
		tok_lex = read_token_from_str(&expansion->a[lex.cur_char], &lex, NULL);
	}

	for (int i=0; i < tlex.size; ++i ) {
		if (tlex.a[i].tok.type == ID) {
			for (int j=0; j < preproc->macros.size; ++j) {
				if (!strcmp(tlex.a[i].tok.v.id, preproc->values.a[j])) {
					p = GET_PARAM(&preproc->params, j);
					if (p->num_params != -1) {
						if (tlex.a[i+1] != LPAREN) 
							continue;
						loc = tlex.a[i].char_pos - expansion->a;
						macro_expansion(preproc, loc, NULL, j);
					} else {
						lex = { 1, 0, 0, tlex.a[i].char_pos };

						tok_lex = read_token(&preproc->values[j][lex.cur_char], &lex, NULL);
						while (tok_lex.tok.type != END) {

						}
					}
				}
			}
		}
	}
	*/

	vector_i valid_macros;
	vec_i(&valid_macros, preproc->macros.size, preproc->macros.size);

	for (int j=0; j < preproc->macros.size; ++j) {
			valid_macros.a[j] = j;
	}

	for (int i=0; i < preproc->macros.size; ++i) {
		search = expansion->a;
		p = GET_PARAM(&preproc->params, i);

		macro_name_len = strlen(preproc->macros.a[i]);
		macro_val_len = strlen(preproc->values.a[i]);

		while (1) {
			found = strstr(search, preproc->macros.a[i]);
			if (!found)
				break;

			//since we should be working in tokens not characters
			//I have to check that the result is not inside another identifier
			//check before and after 
			if (found != expansion->a) {
				if (isalpha(found[-1]) || found[-1] == '_') {
					search = found + macro_name_len;
					continue;
				}
			}

			if (isalnum(found[macro_name_len]) || found[macro_name_len] == '_') {
				search = found + macro_name_len;
				continue;
			}

			if (p->num_params >= 0) {
				ptr = found + macro_name_len;
				while (isspace(*ptr)) ++ptr;
				if (*ptr != '(') {
					search = found + macro_name_len;
					continue;
				}
			}

			loc = found - expansion->a;
			loc = macro_expansion(preproc, expansion, loc, &valid_macros, i);
			search = &expansion->a[loc];
		}
	}

	free_vec_i(&valid_macros);
}

void rescan_expansion(preprocessor_state* preproc, vector_char* expansion, vector_i* valid_macros, int macro_index)
{
	int loc, macro_name_len, macro_val_len;
	char* ptr, *search, *found;
	macro_params* p;

	int macro = valid_macros->a[macro_index];
	erase_i(valid_macros, macro_index, macro_index);

	for (int i, j=0; j < valid_macros->size; ++j) {
		i = valid_macros->a[j];

		search = expansion->a;
		p = GET_PARAM(&preproc->params, i);

		macro_name_len = strlen(preproc->macros.a[i]);
		macro_val_len = strlen(preproc->values.a[i]);
		while (1) {
			found = strstr(search, preproc->macros.a[i]);
			if (!found)
				break;

			if (found != expansion->a) {
				if (isalpha(found[-1]) || found[-1] == '_') {
					search = found + macro_name_len;
					continue;
				}
			}

			if (isalnum(found[macro_name_len]) || found[macro_name_len] == '_') {
				search = found + macro_name_len;
				continue;
			}

			if (p->num_params >= 0) {
				ptr = found + macro_name_len;
				while (isspace(*ptr)) ++ptr;
				if (*ptr != '(') {
					search = found + macro_name_len;
					continue;
				}
			}

			loc = macro_expansion(preproc, expansion, found-expansion->a, valid_macros, j);
			search = &expansion->a[loc];
		}
	}
	insert_i(valid_macros, macro_index, macro);
}


void handle_macro(preprocessor_state* preproc, int macro)
{
	FILE* input = preproc->input;
	FILE* output = preproc->output;
	lexer_state* lexer = &preproc->lexer;

	vector_char expansion;
	init_vec_char(&expansion, preproc->values.a[macro], strlen(preproc->values.a[macro])+1); //+1 for the '\0'

	macro_params* p = GET_PARAM(&preproc->params, macro);
	
	//do initial argument expansion
	if (p->num_params >= 0)
		parse_params(preproc, macro, &expansion);

	vector_i valid_macros;
	vec_i(&valid_macros, preproc->macros.size, preproc->macros.size);
	for (int i=0; i < preproc->macros.size; ++i) {
		valid_macros.a[i] = i;
	}

	rescan_expansion(preproc, &expansion, &valid_macros, macro);

	lexer_state save_lex;
	long fpos;
	token_lex t;

	save_lex = *lexer;
	if ((fpos = ftell(input)) == -1) {
		perror("ftell error in handle_macro");
		exit(0);
	}
	t = read_token(input, lexer, NULL);
	*lexer = save_lex;
	if (fseek(input, fpos, SEEK_SET)) {
		perror("fseek failure in handle_macro");
		exit(0);
	}
	
	if (t.tok.type == LPAREN) { 
		char* search, *found;
		search = expansion.a;

		int len;
		for (int i=0; i < preproc->macros.size; ++i) {
			if (i == macro)
				continue;

			len = strlen(preproc->macros.a[i]);
			search = expansion.a;
			while (1) {
				found = strstr(search, preproc->macros.a[i]);
				if (!found)
					break;
				if (found[len]) {
					search = found + len;
					continue;
				}

				if (found != expansion.a) {
					if (isalpha(found[-1]) || found[-1] == '_') {
						search = found + len;
						continue;
					}
				}
				//don't need to check the end because we already did
				
				p = GET_PARAM(&preproc->params, i);
				if (p->num_params != -1) {
					*found = 0; //cut off matched macro
					fprintf(output, "%s", expansion.a);
					handle_macro(preproc, i);

					goto exit;
				}

				search = found + len;
			}
		}
	} else if (t.tok.type == ID || t.tok.type == STR_LITERAL) {
		free(t.tok.v.id);
	}

	fprintf(output, "%s", expansion.a);

exit:

	free_vec_char(&expansion);
	free_vec_i(&valid_macros);
}


void preprocessor_error(token_lex* tok, lexer_state* lexer, char* str, ...)
{
	va_list args;
	va_start(args, str);

	if (tok) {
		fprintf(stderr, "%s:%u:%u: Preprocessor Error: ", lexer->cur_file, tok->line, tok->pos);
		vfprintf(stderr, str, args);
		fprintf(stderr, " got ");
		print_token(&tok->tok, stderr, 0);
	} else {
		fprintf(stderr, "%s:%u:%u: Preprocessor Error: ", lexer->cur_file, lexer->cur_line, lexer->cur_pos-1);
		vfprintf(stderr, str, args);
	}

	va_end(args);
	exit(0);
}




int look_up_macro_loc(preprocessor_state* proc, char* macro)
{
	for (int i=0; i<proc->macros.size; ++i) {
		if (!strcmp(macro, proc->macros.a[i]))
			return i;
	}
	return -1;
}

char* look_up_macro(preprocessor_state* proc, char* macro)
{
	for (int i=0; i<proc->macros.size; ++i) {
		if (!strcmp(macro, proc->macros.a[i]))
			return proc->values.a[i];
	}
	return NULL;
}

