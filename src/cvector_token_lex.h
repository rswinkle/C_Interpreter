#ifndef CVECTOR_token_lex_H
#define CVECTOR_token_lex_H

#include <stdlib.h>
#include <string.h>

#include "lexer.h"

#ifdef __cplusplus
extern "C" {
#endif


typedef struct cvector_token_lex
{
	token_lex* a;
	size_t size;
	size_t capacity;
	void (*elem_init)(void*, void*);
	void (*elem_free)(void*);
} cvector_token_lex;

extern size_t CVEC_token_lex_SZ;

int cvec_token_lex(cvector_token_lex* vec, size_t size, size_t capacity, void(*elem_free)(void*), void(*elem_init)(void*, void*));
int cvec_init_token_lex(cvector_token_lex* vec, token_lex* vals, size_t num, void(*elem_free)(void*), void(*elem_init)(void*, void*));

cvector_token_lex* cvec_token_lex_heap(size_t size, size_t capacity, void (*elem_free)(void*), void(*elem_init)(void*, void*));
cvector_token_lex* cvec_init_token_lex_heap(token_lex* vals, size_t num, void (*elem_free)(void*), void(*elem_init)(void*, void*));

void cvec_token_lex_copy(void* dest, void* src);

int cvec_push_token_lex(cvector_token_lex* vec, token_lex* val);
void cvec_pop_token_lex(cvector_token_lex* vec, token_lex* ret);

int cvec_extend_token_lex(cvector_token_lex* vec, size_t num);
int cvec_insert_token_lex(cvector_token_lex* vec, size_t i, token_lex* a);
int cvec_insert_array_token_lex(cvector_token_lex* vec, size_t i, token_lex* a, size_t num);
void cvec_replace_token_lex(cvector_token_lex* vec, size_t i, token_lex* a, token_lex* ret);
void cvec_erase_token_lex(cvector_token_lex* vec, size_t start, size_t end);
int cvec_reserve_token_lex(cvector_token_lex* vec, size_t size);
int cvec_set_cap_token_lex(cvector_token_lex* vec, size_t size);
void cvec_set_val_sz_token_lex(cvector_token_lex* vec, token_lex* val);
void cvec_set_val_cap_token_lex(cvector_token_lex* vec, token_lex* val);

token_lex* cvec_back_token_lex(cvector_token_lex* vec);

void cvec_clear_token_lex(cvector_token_lex* vec);
void cvec_free_token_lex_heap(void* vec);
void cvec_free_token_lex(void* vec);

#ifdef __cplusplus
}
#endif


/* CVECTOR_token_lex_H */
#endif

#ifdef CVECTOR_token_lex_IMPLEMENTATION

#include <assert.h>


size_t CVEC_token_lex_SZ = 20;


#define CVEC_token_lex_ALLOCATOR(x) ((x) * 2)






/*  general vector */

cvector_token_lex* cvec_token_lex_heap(size_t size, size_t capacity, void(*elem_free)(void*), void(*elem_init)(void*, void*))
{
	cvector_token_lex* vec;
	if (!(vec = (cvector_token_lex*)malloc(sizeof(cvector_token_lex)))) {
		assert(vec != NULL);
		return NULL;
	}

	vec->size = size;
	vec->capacity = (capacity > vec->size || (vec->size && capacity == vec->size)) ? capacity : vec->size + CVEC_token_lex_SZ;

	/*not calloc here and init_vec as in vector_s because elem_free cannot be calling free directly*/
	if (!(vec->a = (token_lex*)malloc(vec->capacity * sizeof(token_lex)))) {
		assert(vec->a != NULL);
		free(vec);
		return NULL;
	}

	vec->elem_free = elem_free;
	vec->elem_init = elem_init;

	return vec;
}

cvector_token_lex* cvec_init_token_lex_heap(token_lex* vals, size_t num, void(*elem_free)(void*), void(*elem_init)(void*, void*))
{
	cvector_token_lex* vec;
	size_t i;
	
	if (!(vec = (cvector_token_lex*)malloc(sizeof(cvector_token_lex)))) {
		assert(vec != NULL);
		return NULL;
	}

	vec->capacity = num + CVEC_token_lex_SZ;
	vec->size = num;
	if (!(vec->a = (token_lex*)malloc(vec->capacity * sizeof(token_lex)))) {
		assert(vec->a != NULL);
		free(vec);
		return NULL;
	}

	if (elem_init) {
		for (i=0; i<num; ++i) {
			elem_init(&vec->a[i], &vals[i]);
		}
	} else {
		memmove(vec->a, vals, sizeof(token_lex)*num);
	}
	
	vec->elem_free = elem_free;
	vec->elem_init = elem_init;

	return vec;
}

int cvec_token_lex(cvector_token_lex* vec, size_t size, size_t capacity, void(*elem_free)(void*), void(*elem_init)(void*, void*))
{
	vec->size = size;
	vec->capacity = (capacity > vec->size || (vec->size && capacity == vec->size)) ? capacity : vec->size + CVEC_token_lex_SZ;

	if (!(vec->a = (token_lex*)malloc(vec->capacity * sizeof(token_lex)))) {
		assert(vec->a != NULL);
		vec->size = vec->capacity = 0;
		return 0;
	}

	vec->elem_free = elem_free;
	vec->elem_init = elem_init;

	return 1;
}

int cvec_init_token_lex(cvector_token_lex* vec, token_lex* vals, size_t num, void(*elem_free)(void*), void(*elem_init)(void*, void*))
{
	size_t i;
	
	vec->capacity = num + CVEC_token_lex_SZ;
	vec->size = num;
	if (!(vec->a = (token_lex*)malloc(vec->capacity * sizeof(token_lex)))) {
		assert(vec->a != NULL);
		vec->size = vec->capacity = 0;
		return 0;
	}

	if (elem_init) {
		for (i=0; i<num; ++i) {
			elem_init(&vec->a[i], &vals[i]);
		}
	} else {
		memmove(vec->a, vals, sizeof(token_lex)*num);
	}

	vec->elem_free = elem_free;
	vec->elem_init = elem_init;

	return 1;
}


void cvec_token_lex_copy(void* dest, void* src)
{
	size_t i;
	cvector_token_lex* vec1 = (cvector_token_lex*)dest;
	cvector_token_lex* vec2 = (cvector_token_lex*)src;
	
	vec1->size = 0;
	vec1->capacity = 0;
	
	/*not much else we can do here*/
	if (!(vec1->a = (token_lex*)malloc(vec2->capacity*sizeof(token_lex)))) {
		assert(vec1->a != NULL);
		return;
	}

	vec1->size = vec2->size;
	vec1->capacity = vec2->capacity;
	vec1->elem_init = vec2->elem_init;
	vec1->elem_free = vec2->elem_free;
	
	if (vec1->elem_init) {
		for (i=0; i<vec1->size; ++i) {
			vec1->elem_init(&vec1->a[i], &vec2->a[i]);
		}
	} else {
		memmove(vec1->a, vec2->a, vec1->size*sizeof(token_lex));
	}
}

int cvec_push_token_lex(cvector_token_lex* vec, token_lex* a)
{
	token_lex* tmp;
	size_t tmp_sz;
	if (vec->capacity > vec->size) {
		if (vec->elem_init) {
			vec->elem_init(&vec->a[vec->size], a);
		} else {
			memmove(&vec->a[vec->size], a, sizeof(token_lex));
		}
	} else {
		tmp_sz = CVEC_token_lex_ALLOCATOR(vec->capacity);
		if (!(tmp = (token_lex*)realloc(vec->a, sizeof(token_lex)*tmp_sz))) {
			assert(tmp != NULL);
			return 0;
		}
		vec->a = tmp;
		
		if (vec->elem_init) {
			vec->elem_init(&vec->a[vec->size], a);
		} else {
			memmove(&vec->a[vec->size], a, sizeof(token_lex));
		}
		
		vec->capacity = tmp_sz;
	}
	
	vec->size++;
	return 1;
}

void cvec_pop_token_lex(cvector_token_lex* vec, token_lex* ret)
{
	if (ret) {
		memmove(ret, &vec->a[--vec->size], sizeof(token_lex));
	} else {
		vec->size--;
	}

	if (vec->elem_free) {
		vec->elem_free(&vec->a[vec->size]);
	}
}

/** Return pointer to last element */
token_lex* cvec_back_token_lex(cvector_token_lex* vec)
{
	return &vec->a[vec->size-1];
}

int cvec_extend_token_lex(cvector_token_lex* vec, size_t num)
{
	token_lex* tmp;
	size_t tmp_sz;
	if (vec->capacity < vec->size + num) {
		tmp_sz = vec->capacity + num + CVEC_token_lex_SZ;
		if (!(tmp = (token_lex*)realloc(vec->a, sizeof(token_lex)*tmp_sz))) {
			assert(tmp != NULL);
			return 0;
		}
		vec->a = tmp;
		vec->capacity = tmp_sz;
	}

	vec->size += num;
	return 1;
}

int cvec_insert_token_lex(cvector_token_lex* vec, size_t i, token_lex* a)
{
	token_lex* tmp;
	size_t tmp_sz;
	if (vec->capacity > vec->size) {
		memmove(&vec->a[i+1], &vec->a[i], (vec->size-i)*sizeof(token_lex));

		if (vec->elem_init) {
			vec->elem_init(&vec->a[i], a);
		} else {
			memmove(&vec->a[i], a, sizeof(token_lex));
		}
	} else {
		tmp_sz = CVEC_token_lex_ALLOCATOR(vec->capacity);
		if (!(tmp = (token_lex*)realloc(vec->a, sizeof(token_lex)*tmp_sz))) {
			assert(tmp != NULL);
			return 0;
		}
		
		vec->a = tmp;
		memmove(&vec->a[i+1], &vec->a[i], (vec->size-i)*sizeof(token_lex));
		
		if (vec->elem_init) {
			vec->elem_init(&vec->a[i], a);
		} else {
			memmove(&vec->a[i], a, sizeof(token_lex));
		}
		
		vec->capacity = tmp_sz;
	}

	vec->size++;
	return 1;
}

int cvec_insert_array_token_lex(cvector_token_lex* vec, size_t i, token_lex* a, size_t num)
{
	token_lex* tmp;
	size_t tmp_sz, j;
	if (vec->capacity < vec->size + num) {
		tmp_sz = vec->capacity + num + CVEC_token_lex_SZ;
		if (!(tmp = (token_lex*)realloc(vec->a, sizeof(token_lex)*tmp_sz))) {
			assert(tmp != NULL);
			return 0;
		}
		vec->a = tmp;
		vec->capacity = tmp_sz;
	}

	memmove(&vec->a[i+num], &vec->a[i], (vec->size-i)*sizeof(token_lex));
	if (vec->elem_init) {
		for (j=0; j<num; ++j) {
			vec->elem_init(&vec->a[j+i], &a[j]);
		}
	} else {
		memmove(&vec->a[i], a, num*sizeof(token_lex));
	}
	vec->size += num;
	return 1;
}

void cvec_replace_token_lex(cvector_token_lex* vec, size_t i, token_lex* a, token_lex* ret)
{
	if (ret)
		memmove(ret, &vec->a[i], sizeof(token_lex));
	memmove(&vec->a[i], a, sizeof(token_lex));
}

void cvec_erase_token_lex(cvector_token_lex* vec, size_t start, size_t end)
{
	size_t i;
	size_t d = end - start + 1;
	if (vec->elem_free) {
		for (i=start; i<=end; i++) {
			vec->elem_free(&vec->a[i]);
		}
	}
	memmove(&vec->a[start], &vec->a[end+1], (vec->size-1-end)*sizeof(token_lex));
	vec->size -= d;
}

int cvec_reserve_token_lex(cvector_token_lex* vec, size_t size)
{
	token_lex* tmp;
	if (vec->capacity < size) {
		if (!(tmp = (token_lex*)realloc(vec->a, sizeof(token_lex)*(size+CVEC_token_lex_SZ)))) {
			assert(tmp != NULL);
			return 0;
		}
		vec->a = tmp;
		vec->capacity = size + CVEC_token_lex_SZ;
	}
	return 1;
}

int cvec_set_cap_token_lex(cvector_token_lex* vec, size_t size)
{
	size_t i;
	token_lex* tmp;
	if (size < vec->size) {
		if (vec->elem_free) {
			for (i=vec->size-1; i>=size; i--) {
				vec->elem_free(&vec->a[i]);
			}
		}
		vec->size = size;
	}

	vec->capacity = size;

	if (!(tmp = (token_lex*)realloc(vec->a, sizeof(token_lex)*size))) {
		assert(tmp != NULL);
		return 0;
	}
	vec-> a = tmp;
	return 1;
}

void cvec_set_val_sz_token_lex(cvector_token_lex* vec, token_lex* val)
{
	size_t i;

	if (vec->elem_free) {
		for(i=0; i<vec->size; i++) {
			vec->elem_free(&vec->a[i]);
		}
	}
	
	if (vec->elem_init) {
		for (i=0; i<vec->size; i++) {
			vec->elem_init(&vec->a[i], val);
		}
	} else {
		for (i=0; i<vec->size; i++) {
			memmove(&vec->a[i], val, sizeof(token_lex));
		}
	}
}

void cvec_set_val_cap_token_lex(cvector_token_lex* vec, token_lex* val)
{
	size_t i;
	if (vec->elem_free) {
		for (i=0; i<vec->size; i++) {
			vec->elem_free(&vec->a[i]);
		}
		vec->size = vec->capacity;
	}

	if (vec->elem_init) {
		for (i=0; i<vec->capacity; i++) {
			vec->elem_init(&vec->a[i], val);
		}
	} else {
		for (i=0; i<vec->capacity; i++) {
			memmove(&vec->a[i], val, sizeof(token_lex));
		}
	}
}

void cvec_clear_token_lex(cvector_token_lex* vec)
{
	size_t i;
	if (vec->elem_free) {
		for (i=0; i<vec->size; ++i) {
			vec->elem_free(&vec->a[i]);
		}
	}
	vec->size = 0;
}

void cvec_free_token_lex_heap(void* vec)
{
	size_t i;
	cvector_token_lex* tmp = (cvector_token_lex*)vec;
	if (tmp->elem_free) {
		for (i=0; i<tmp->size; i++) {
			tmp->elem_free(&tmp->a[i]);
		}
	}
	free(tmp->a);
	free(tmp);
}

void cvec_free_token_lex(void* vec)
{
	size_t i;
	cvector_token_lex* tmp = (cvector_token_lex*)vec;
	if (tmp->elem_free) {
		for (i=0; i<tmp->size; i++) {
			tmp->elem_free(&tmp->a[i]);
		}
	}

	free(tmp->a);

	tmp->size = 0;
	tmp->capacity = 0;
}


#endif
