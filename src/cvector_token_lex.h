#ifndef CVECTOR_token_lex_H
#define CVECTOR_token_lex_H

#include <stdlib.h>

#include "lexer.h"

#ifdef __cplusplus
extern "C" {
#endif


typedef struct cvector_token_lex
{
	token_lex* a;
	size_t size;
	size_t capacity;
	void (*elem_free)(void*);
	int (*elem_init)(void*, void*);
} cvector_token_lex;

extern size_t CVEC_token_lex_SZ;

int cvec_token_lex(cvector_token_lex* vec, size_t size, size_t capacity, void(*elem_free)(void*), int(*elem_init)(void*, void*));
int cvec_init_token_lex(cvector_token_lex* vec, token_lex* vals, size_t num, void(*elem_free)(void*), int(*elem_init)(void*, void*));

cvector_token_lex* cvec_token_lex_heap(size_t size, size_t capacity, void (*elem_free)(void*), int(*elem_init)(void*, void*));
cvector_token_lex* cvec_init_token_lex_heap(token_lex* vals, size_t num, void (*elem_free)(void*), int(*elem_init)(void*, void*));

int cvec_copyc_token_lex(void* dest, void* src);
int cvec_copy_token_lex(cvector_token_lex* dest, cvector_token_lex* src);

int cvec_push_token_lex(cvector_token_lex* vec, token_lex* val);
void cvec_pop_token_lex(cvector_token_lex* vec, token_lex* ret);

int cvec_pushm_token_lex(cvector_token_lex* vec, token_lex* a);
void cvec_popm_token_lex(cvector_token_lex* vec, token_lex* ret);
int cvec_insertm_token_lex(cvector_token_lex* vec, size_t i, token_lex* a);
int cvec_insert_arraym_token_lex(cvector_token_lex* vec, size_t i, token_lex* a, size_t num);
void cvec_replacem_token_lex(cvector_token_lex* vec, size_t i, token_lex* a, token_lex* ret);

int cvec_extend_token_lex(cvector_token_lex* vec, size_t num);
int cvec_insert_token_lex(cvector_token_lex* vec, size_t i, token_lex* a);
int cvec_insert_array_token_lex(cvector_token_lex* vec, size_t i, token_lex* a, size_t num);
int cvec_replace_token_lex(cvector_token_lex* vec, size_t i, token_lex* a, token_lex* ret);
void cvec_erase_token_lex(cvector_token_lex* vec, size_t start, size_t end);
void cvec_remove_token_lex(cvector_token_lex* vec, size_t start, size_t end);
int cvec_reserve_token_lex(cvector_token_lex* vec, size_t size);
int cvec_set_cap_token_lex(cvector_token_lex* vec, size_t size);
int cvec_set_val_sz_token_lex(cvector_token_lex* vec, token_lex* val);
int cvec_set_val_cap_token_lex(cvector_token_lex* vec, token_lex* val);

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



size_t CVEC_token_lex_SZ = 20;

#define CVEC_token_lex_ALLOCATOR(x) ((x+1) * 2)

#if defined(CVEC_MALLOC) && defined(CVEC_FREE) && defined(CVEC_REALLOC)
/* ok */
#elif !defined(CVEC_MALLOC) && !defined(CVEC_FREE) && !defined(CVEC_REALLOC)
/* ok */
#else
#error "Must define all or none of CVEC_MALLOC, CVEC_FREE, and CVEC_REALLOC."
#endif

#ifndef CVEC_MALLOC
#define CVEC_MALLOC(sz)      malloc(sz)
#define CVEC_REALLOC(p, sz)  realloc(p, sz)
#define CVEC_FREE(p)         free(p)
#endif

#ifndef CVEC_MEMMOVE
#include <string.h>
#define CVEC_MEMMOVE(dst, src, sz)  memmove(dst, src, sz)
#endif

#ifndef CVEC_ASSERT
#include <assert.h>
#define CVEC_ASSERT(x)       assert(x)
#endif


/*  general vector */

cvector_token_lex* cvec_token_lex_heap(size_t size, size_t capacity, void(*elem_free)(void*), int(*elem_init)(void*, void*))
{
	cvector_token_lex* vec;
	if (!(vec = (cvector_token_lex*)CVEC_MALLOC(sizeof(cvector_token_lex)))) {
		CVEC_ASSERT(vec != NULL);
		return NULL;
	}

	vec->size = size;
	vec->capacity = (capacity > vec->size || (vec->size && capacity == vec->size)) ? capacity : vec->size + CVEC_token_lex_SZ;

	if (!(vec->a = (token_lex*)CVEC_MALLOC(vec->capacity * sizeof(token_lex)))) {
		CVEC_ASSERT(vec->a != NULL);
		CVEC_FREE(vec);
		return NULL;
	}

	vec->elem_free = elem_free;
	vec->elem_init = elem_init;

	return vec;
}

cvector_token_lex* cvec_init_token_lex_heap(token_lex* vals, size_t num, void(*elem_free)(void*), int(*elem_init)(void*, void*))
{
	cvector_token_lex* vec;
	size_t i;
	
	if (!(vec = (cvector_token_lex*)CVEC_MALLOC(sizeof(cvector_token_lex)))) {
		CVEC_ASSERT(vec != NULL);
		return NULL;
	}

	vec->capacity = num + CVEC_token_lex_SZ;
	vec->size = num;
	if (!(vec->a = (token_lex*)CVEC_MALLOC(vec->capacity * sizeof(token_lex)))) {
		CVEC_ASSERT(vec->a != NULL);
		CVEC_FREE(vec);
		return NULL;
	}

	if (elem_init) {
		for (i=0; i<num; ++i) {
			if (!elem_init(&vec->a[i], &vals[i])) {
				CVEC_ASSERT(0);
				CVEC_FREE(vec->a);
				CVEC_FREE(vec);
				return NULL;
			}
		}
	} else {
		CVEC_MEMMOVE(vec->a, vals, sizeof(token_lex)*num);
	}
	
	vec->elem_free = elem_free;
	vec->elem_init = elem_init;

	return vec;
}

int cvec_token_lex(cvector_token_lex* vec, size_t size, size_t capacity, void(*elem_free)(void*), int(*elem_init)(void*, void*))
{
	vec->size = size;
	vec->capacity = (capacity > vec->size || (vec->size && capacity == vec->size)) ? capacity : vec->size + CVEC_token_lex_SZ;

	if (!(vec->a = (token_lex*)CVEC_MALLOC(vec->capacity * sizeof(token_lex)))) {
		CVEC_ASSERT(vec->a != NULL);
		vec->size = vec->capacity = 0;
		return 0;
	}

	vec->elem_free = elem_free;
	vec->elem_init = elem_init;

	return 1;
}

int cvec_init_token_lex(cvector_token_lex* vec, token_lex* vals, size_t num, void(*elem_free)(void*), int(*elem_init)(void*, void*))
{
	size_t i;
	
	vec->capacity = num + CVEC_token_lex_SZ;
	vec->size = num;
	if (!(vec->a = (token_lex*)CVEC_MALLOC(vec->capacity * sizeof(token_lex)))) {
		CVEC_ASSERT(vec->a != NULL);
		vec->size = vec->capacity = 0;
		return 0;
	}

	if (elem_init) {
		for (i=0; i<num; ++i) {
			if (!elem_init(&vec->a[i], &vals[i])) {
				CVEC_ASSERT(0);
				return 0;
			}
		}
	} else {
		CVEC_MEMMOVE(vec->a, vals, sizeof(token_lex)*num);
	}

	vec->elem_free = elem_free;
	vec->elem_init = elem_init;

	return 1;
}

int cvec_copyc_token_lex(void* dest, void* src)
{
	cvector_token_lex* vec1 = (cvector_token_lex*)dest;
	cvector_token_lex* vec2 = (cvector_token_lex*)src;

	vec1->a = NULL;
	vec1->size = 0;
	vec1->capacity = 0;

	return cvec_copy_token_lex(vec1, vec2);
}

int cvec_copy_token_lex(cvector_token_lex* dest, cvector_token_lex* src)
{
	int i;
	token_lex* tmp = NULL;
	if (!(tmp = (token_lex*)CVEC_REALLOC(dest->a, src->capacity*sizeof(token_lex)))) {
		CVEC_ASSERT(tmp != NULL);
		return 0;
	}
	dest->a = tmp;

	if (src->elem_init) {
		for (i=0; i<src->size; ++i) {
			if (!src->elem_init(&dest->a[i], &src->a[i])) {
				CVEC_ASSERT(0);
				return 0;
			}
		}
	} else {
		/* could use memcpy here since we know we just allocated dest->a */
		CVEC_MEMMOVE(dest->a, src->a, src->size*sizeof(token_lex));
	}

	dest->size = src->size;
	dest->capacity = src->capacity;
	dest->elem_free = src->elem_free;
	dest->elem_init = src->elem_init;
	return 1;
}

int cvec_push_token_lex(cvector_token_lex* vec, token_lex* a)
{
	token_lex* tmp;
	size_t tmp_sz;
	if (vec->capacity == vec->size) {
		tmp_sz = CVEC_token_lex_ALLOCATOR(vec->capacity);
		if (!(tmp = (token_lex*)CVEC_REALLOC(vec->a, sizeof(token_lex)*tmp_sz))) {
			CVEC_ASSERT(tmp != NULL);
			return 0;
		}
		vec->a = tmp;
		vec->capacity = tmp_sz;
	}
	if (vec->elem_init) {
		if (!vec->elem_init(&vec->a[vec->size], a)) {
			CVEC_ASSERT(0);
			return 0;
		}
	} else {
		CVEC_MEMMOVE(&vec->a[vec->size], a, sizeof(token_lex));
	}
	
	vec->size++;
	return 1;
}

int cvec_pushm_token_lex(cvector_token_lex* vec, token_lex* a)
{
	token_lex* tmp;
	size_t tmp_sz;
	if (vec->capacity == vec->size) {
		tmp_sz = CVEC_token_lex_ALLOCATOR(vec->capacity);
		if (!(tmp = (token_lex*)CVEC_REALLOC(vec->a, sizeof(token_lex)*tmp_sz))) {
			CVEC_ASSERT(tmp != NULL);
			return 0;
		}
		vec->a = tmp;
		vec->capacity = tmp_sz;
	}
	CVEC_MEMMOVE(&vec->a[vec->size], a, sizeof(token_lex));
	
	vec->size++;
	return 1;
}

void cvec_pop_token_lex(cvector_token_lex* vec, token_lex* ret)
{
	vec->size--;
	if (ret) {
		CVEC_MEMMOVE(ret, &vec->a[vec->size], sizeof(token_lex));
	}

	if (vec->elem_free) {
		vec->elem_free(&vec->a[vec->size]);
	}
}

void cvec_popm_token_lex(cvector_token_lex* vec, token_lex* ret)
{
	vec->size--;
	if (ret) {
		CVEC_MEMMOVE(ret, &vec->a[vec->size], sizeof(token_lex));
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
		if (!(tmp = (token_lex*)CVEC_REALLOC(vec->a, sizeof(token_lex)*tmp_sz))) {
			CVEC_ASSERT(tmp != NULL);
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
	if (vec->capacity == vec->size) {
		tmp_sz = CVEC_token_lex_ALLOCATOR(vec->capacity);
		if (!(tmp = (token_lex*)CVEC_REALLOC(vec->a, sizeof(token_lex)*tmp_sz))) {
			CVEC_ASSERT(tmp != NULL);
			return 0;
		}
		
		vec->a = tmp;
		vec->capacity = tmp_sz;
	}
	CVEC_MEMMOVE(&vec->a[i+1], &vec->a[i], (vec->size-i)*sizeof(token_lex));

	if (vec->elem_init) {
		if (!vec->elem_init(&vec->a[i], a)) {
			CVEC_ASSERT(0);
			return 0;
		}
	} else {
		CVEC_MEMMOVE(&vec->a[i], a, sizeof(token_lex));
	}

	vec->size++;
	return 1;
}

int cvec_insertm_token_lex(cvector_token_lex* vec, size_t i, token_lex* a)
{
	token_lex* tmp;
	size_t tmp_sz;
	if (vec->capacity == vec->size) {
		tmp_sz = CVEC_token_lex_ALLOCATOR(vec->capacity);
		if (!(tmp = (token_lex*)CVEC_REALLOC(vec->a, sizeof(token_lex)*tmp_sz))) {
			CVEC_ASSERT(tmp != NULL);
			return 0;
		}
		
		vec->a = tmp;
		vec->capacity = tmp_sz;
	}
	CVEC_MEMMOVE(&vec->a[i+1], &vec->a[i], (vec->size-i)*sizeof(token_lex));

	CVEC_MEMMOVE(&vec->a[i], a, sizeof(token_lex));

	vec->size++;
	return 1;
}

int cvec_insert_array_token_lex(cvector_token_lex* vec, size_t i, token_lex* a, size_t num)
{
	token_lex* tmp;
	size_t tmp_sz, j;
	if (vec->capacity < vec->size + num) {
		tmp_sz = vec->capacity + num + CVEC_token_lex_SZ;
		if (!(tmp = (token_lex*)CVEC_REALLOC(vec->a, sizeof(token_lex)*tmp_sz))) {
			CVEC_ASSERT(tmp != NULL);
			return 0;
		}
		vec->a = tmp;
		vec->capacity = tmp_sz;
	}

	CVEC_MEMMOVE(&vec->a[i+num], &vec->a[i], (vec->size-i)*sizeof(token_lex));
	if (vec->elem_init) {
		for (j=0; j<num; ++j) {
			if (!vec->elem_init(&vec->a[j+i], &a[j])) {
				CVEC_ASSERT(0);
				return 0;
			}
		}
	} else {
		CVEC_MEMMOVE(&vec->a[i], a, num*sizeof(token_lex));
	}
	vec->size += num;
	return 1;
}

int cvec_insert_arraym_token_lex(cvector_token_lex* vec, size_t i, token_lex* a, size_t num)
{
	token_lex* tmp;
	size_t tmp_sz;
	if (vec->capacity < vec->size + num) {
		tmp_sz = vec->capacity + num + CVEC_VOID_START_SZ;
		if (!(tmp = (token_lex*)CVEC_REALLOC(vec->a, sizeof(token_lex)*tmp_sz))) {
			CVEC_ASSERT(tmp != NULL);
			return 0;
		}
		vec->a = tmp;
		vec->capacity = tmp_sz;
	}

	CVEC_MEMMOVE(&vec->a[i+num], &vec->a[i], (vec->size-i)*sizeof(token_lex));

	CVEC_MEMMOVE(&vec->a[i], a, num*sizeof(token_lex));
	vec->size += num;
	return 1;
}

int cvec_replace_token_lex(cvector_token_lex* vec, size_t i, token_lex* a, token_lex* ret)
{
	if (ret) {
		CVEC_MEMMOVE(ret, &vec->a[i], sizeof(token_lex));
	} else if (vec->elem_free) {
		vec->elem_free(&vec->a[i]);
	}

	if (vec->elem_init) {
		if (!vec->elem_init(&vec->a[i], a)) {
			CVEC_ASSERT(0);
			return 0;
		}
	} else {
		CVEC_MEMMOVE(&vec->a[i], a, sizeof(token_lex));
	}
	return 1;
}

void cvec_replacem_token_lex(cvector_token_lex* vec, size_t i, token_lex* a, token_lex* ret)
{
	if (ret) {
		CVEC_MEMMOVE(ret, &vec->a[i], sizeof(token_lex));
	}

	CVEC_MEMMOVE(&vec->a[i], a, sizeof(token_lex));
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
	CVEC_MEMMOVE(&vec->a[start], &vec->a[end+1], (vec->size-1-end)*sizeof(token_lex));
	vec->size -= d;
}

void cvec_remove_token_lex(cvector_token_lex* vec, size_t start, size_t end)
{
	size_t d = end - start + 1;
	CVEC_MEMMOVE(&vec->a[start], &vec->a[end+1], (vec->size-1-end)*sizeof(token_lex));
	vec->size -= d;
}

int cvec_reserve_token_lex(cvector_token_lex* vec, size_t size)
{
	token_lex* tmp;
	if (vec->capacity < size) {
		if (!(tmp = (token_lex*)CVEC_REALLOC(vec->a, sizeof(token_lex)*(size+CVEC_token_lex_SZ)))) {
			CVEC_ASSERT(tmp != NULL);
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

	if (!(tmp = (token_lex*)CVEC_REALLOC(vec->a, sizeof(token_lex)*size))) {
		CVEC_ASSERT(tmp != NULL);
		return 0;
	}
	vec-> a = tmp;
	return 1;
}

int cvec_set_val_sz_token_lex(cvector_token_lex* vec, token_lex* val)
{
	size_t i;

	if (vec->elem_free) {
		for(i=0; i<vec->size; i++) {
			vec->elem_free(&vec->a[i]);
		}
	}
	
	if (vec->elem_init) {
		for (i=0; i<vec->size; i++) {
			if (!vec->elem_init(&vec->a[i], val)) {
				CVEC_ASSERT(0);
				return 0;
			}
		}
	} else {
		for (i=0; i<vec->size; i++) {
			CVEC_MEMMOVE(&vec->a[i], val, sizeof(token_lex));
		}
	}
	return 1;
}

int cvec_set_val_cap_token_lex(cvector_token_lex* vec, token_lex* val)
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
			if (!vec->elem_init(&vec->a[i], val)) {
				CVEC_ASSERT(0);
				return 0;
			}
		}
	} else {
		for (i=0; i<vec->capacity; i++) {
			CVEC_MEMMOVE(&vec->a[i], val, sizeof(token_lex));
		}
	}
	return 1;
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
	if (!tmp) return;
	if (tmp->elem_free) {
		for (i=0; i<tmp->size; i++) {
			tmp->elem_free(&tmp->a[i]);
		}
	}
	CVEC_FREE(tmp->a);
	CVEC_FREE(tmp);
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

	CVEC_FREE(tmp->a);

	tmp->size = 0;
	tmp->capacity = 0;
}


#endif
