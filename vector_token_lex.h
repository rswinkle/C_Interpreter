#ifndef VECTOR_token_lex_H
#define VECTOR_token_lex_H

#include "lexer.h"

#include <stdlib.h>
#include <string.h>
#include <stdio.h>


//struct token_lex;

typedef struct vector_token_lex
{
	token_lex* a;
	size_t size;
	size_t capacity;
	void (*elem_init)(void*, void*);
	void (*elem_free)(void*);
} vector_token_lex;

extern size_t VEC_token_lex_SZ;

int vec_token_lex(vector_token_lex* vec, size_t size, size_t capacity, void(*elem_free)(void*), void(*elem_init)(void*, void*));
int init_vec_token_lex(vector_token_lex* vec, token_lex* vals, size_t num, void(*elem_free)(void*), void(*elem_init)(void*, void*));

vector_token_lex* vec_token_lex_heap(size_t size, size_t capacity, void (*elem_free)(void*), void(*elem_init)(void*, void*));
vector_token_lex* init_vec_token_lex_heap(token_lex* vals, size_t num, void (*elem_free)(void*), void(*elem_init)(void*, void*));

void vec_token_lex_copy(void* dest, void* src);

int push_token_lex(vector_token_lex* vec, token_lex* val);
void pop_token_lex(vector_token_lex* vec, token_lex* ret);

int extend_token_lex(vector_token_lex* vec, size_t num);
int insert_token_lex(vector_token_lex* vec, size_t i, token_lex* a);
int insert_array_token_lex(vector_token_lex* vec, size_t i, token_lex* a, size_t num);
void erase_token_lex(vector_token_lex* vec, size_t start, size_t end);
int reserve_token_lex(vector_token_lex* vec, size_t size);
int set_capacity_token_lex(vector_token_lex* vec, size_t size);
void set_val_sz_token_lex(vector_token_lex* vec, token_lex* val);
void set_val_cap_token_lex(vector_token_lex* vec, token_lex* val);

token_lex* back_token_lex(vector_token_lex* vec);

void clear_token_lex(vector_token_lex* vec);
void free_vec_token_lex_heap(void* vec);
void free_vec_token_lex(void* vec);



#endif
