#include "vector_token_lex.h"

#include <assert.h>

#define STDERR(X) fprintf(stderr, X)


size_t VEC_token_lex_SZ = 20;


#define VEC_token_lex_ALLOCATOR(x) ((x) * 2)






/*  general vector */

vector_token_lex* vec_token_lex_heap(size_t size, size_t capacity, void(*elem_free)(void*), void(*elem_init)(void*, void*))
{
	vector_token_lex* vec;
	if (!(vec = malloc(sizeof(vector_token_lex)))) {
		assert(vec != NULL);
		return NULL;
	}

	vec->size = (size > 0) ? size : 0;
	vec->capacity = (capacity > vec->size || (vec->size && capacity == vec->size)) ? capacity : vec->size + VEC_token_lex_SZ;

	/*not calloc here and init_vec as in vector_s because elem_free cannot be calling free directly*/
	if (!(vec->a = malloc(vec->capacity * sizeof(token_lex)))) {
		assert(vec->a != NULL);
		free(vec);
		return NULL;
	}

	vec->elem_free = elem_free;
	vec->elem_init = elem_init;

	return vec;
}



vector_token_lex* init_vec_token_lex_heap(token_lex* vals, size_t num, void(*elem_free)(void*), void(*elem_init)(void*, void*))
{
	vector_token_lex* vec;
	size_t i;
	
	if (!(vec = malloc(sizeof(vector_token_lex)))) {
		assert(vec != NULL);
		return NULL;
	}

	vec->capacity = num + VEC_token_lex_SZ;
	vec->size = num;
	if (!(vec->a = malloc(vec->capacity * sizeof(token_lex)))) {
		assert(vec->a != NULL);
		free(vec);
		return NULL;
	}

	if (elem_init) {
		for (i=0; i<num; ++i) {
			elem_init(&vec->a[i], &vals[i]);
		}
	} else {
		memcpy(vec->a, vals, sizeof(token_lex)*num);
	}
	
	vec->elem_free = elem_free;
	vec->elem_init = elem_init;

	return vec;
}

int vec_token_lex(vector_token_lex* vec, size_t size, size_t capacity, void(*elem_free)(void*), void(*elem_init)(void*, void*))
{
	vec->size = (size > 0) ? size : 0;
	vec->capacity = (capacity > vec->size || (vec->size && capacity == vec->size)) ? capacity : vec->size + VEC_token_lex_SZ;

	if (!(vec->a = malloc(vec->capacity * sizeof(token_lex)))) {
		assert(vec->a != NULL);
		vec->size = vec->capacity = 0;
		return 0;
	}

	vec->elem_free = elem_free;
	vec->elem_init = elem_init;

	return 1;
}

int init_vec_token_lex(vector_token_lex* vec, token_lex* vals, size_t num, void(*elem_free)(void*), void(*elem_init)(void*, void*))
{
	size_t i;
	
	vec->capacity = num + VEC_token_lex_SZ;
	vec->size = num;
	if (!(vec->a = malloc(vec->capacity * sizeof(token_lex)))) {
		assert(vec->a != NULL);
		vec->size = vec->capacity = 0;
		return 0;
	}

	if (elem_init) {
		for (i=0; i<num; ++i) {
			elem_init(&vec->a[i], &vals[i]);
		}
	} else {
		memcpy(vec->a, vals, sizeof(token_lex)*num);
	}

	vec->elem_free = elem_free;
	vec->elem_init = elem_init;

	return 1;
}


void vec_token_lex_copy(void* dest, void* src)
{
	size_t i;
	vector_token_lex* vec1 = dest;
	vector_token_lex* vec2 = src;
	
	vec1->size = 0;
	vec1->capacity = 0;
	
	/*not much else we can do here*/
	if (!(vec1->a = malloc(vec2->capacity*sizeof(token_lex)))) {
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
		memcpy(vec1->a, vec2->a, vec1->size*sizeof(token_lex));
	}
}


int push_token_lex(vector_token_lex* vec, token_lex* a)
{
	void* tmp;
	size_t tmp_sz;
	if (vec->capacity > vec->size) {
		if (vec->elem_init) {
			vec->elem_init(&vec->a[vec->size], a);
		} else {
			memcpy(&vec->a[vec->size], a, sizeof(token_lex));
		}
	} else {
		tmp_sz = VEC_token_lex_ALLOCATOR(vec->capacity);
		if (!(tmp = realloc(vec->a, sizeof(token_lex)*tmp_sz))) {
			assert(tmp != NULL);
			return 0;
		}
		vec->a = tmp;
		
		if (vec->elem_init) {
			vec->elem_init(&vec->a[vec->size], a);
		} else {
			memcpy(&vec->a[vec->size], a, sizeof(token_lex));
		}
		
		vec->capacity = tmp_sz;
	}
	
	vec->size++;
	return 1;
}


void pop_token_lex(vector_token_lex* vec, token_lex* ret)
{
	if (ret) {
		memcpy(ret, &vec->a[--vec->size], sizeof(token_lex));
	} else {
		vec->size--;
	}

	if (vec->elem_free) {
		vec->elem_free(&vec->a[vec->size]);
	}
}

/** Return pointer to last element */
token_lex* back_token_lex(vector_token_lex* vec)
{
	return &vec->a[vec->size-1];
}




int extend_token_lex(vector_token_lex* vec, size_t num)
{
	void* tmp;
	size_t tmp_sz;
	if (vec->capacity < vec->size + num) {
		tmp_sz = vec->capacity + num + VEC_token_lex_SZ;
		if (!(tmp = realloc(vec->a, sizeof(token_lex)*tmp_sz))) {
			assert(tmp != NULL);
			return 0;
		}
		vec->a = tmp;
		vec->capacity = tmp_sz;
	}

	vec->size += num;
	return 1;
}


int insert_token_lex(vector_token_lex* vec, size_t i, token_lex* a)
{
	void* tmp;
	size_t tmp_sz;
	if (vec->capacity > vec->size) {
		memmove(&vec->a[i+1], &vec->a[i], (vec->size-i)*sizeof(token_lex));

		if (vec->elem_init) {
			vec->elem_init(&vec->a[i], a);
		} else {
			memcpy(&vec->a[i], a, sizeof(token_lex));
		}
	} else {
		tmp_sz = VEC_token_lex_ALLOCATOR(vec->capacity);
		if (!(tmp = realloc(vec->a, sizeof(token_lex)*tmp_sz))) {
			assert(tmp != NULL);
			return 0;
		}
		
		vec->a = tmp;
		memmove(&vec->a[i+1], &vec->a[i], (vec->size-i)*sizeof(token_lex));
		
		if (vec->elem_init) {
			vec->elem_init(&vec->a[i], a);
		} else {
			memcpy(&vec->a[i], a, sizeof(token_lex));
		}
		
		vec->capacity = tmp_sz;
	}

	vec->size++;
	return 1;
}

int insert_array_token_lex(vector_token_lex* vec, size_t i, token_lex* a, size_t num)
{
	void* tmp;
	size_t tmp_sz, j;
	if (vec->capacity < vec->size + num) {
		tmp_sz = vec->capacity + num + VEC_token_lex_SZ;
		if (!(tmp = realloc(vec->a, sizeof(token_lex)*tmp_sz))) {
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
		memcpy(&vec->a[i], a, num*sizeof(token_lex));
	}
	vec->size += num;
	return 1;
}


void erase_token_lex(vector_token_lex* vec, size_t start, size_t end)
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


int reserve_token_lex(vector_token_lex* vec, size_t size)
{
	void* tmp;
	if (vec->capacity < size) {
		if (!(tmp = realloc(vec->a, sizeof(token_lex)*(size+VEC_token_lex_SZ)))) {
			assert(tmp != NULL);
			return 0;
		}
		vec->a = tmp;
		vec->capacity = size + VEC_token_lex_SZ;
	}
	return 1;
}


int set_capacity_token_lex(vector_token_lex* vec, size_t size)
{
	size_t i;
	void* tmp;
	if (size < vec->size) {
		if (vec->elem_free) {
			for (i=vec->size-1; i>=size; i--) {
				vec->elem_free(&vec->a[i]);
			}
		}
		vec->size = size;
	}

	vec->capacity = size;

	if (!(tmp = realloc(vec->a, sizeof(token_lex)*size))) {
		assert(tmp != NULL);
		return 0;
	}
	vec-> a = tmp;
	return 1;
}



void set_val_sz_token_lex(vector_token_lex* vec, token_lex* val)
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
			memcpy(&vec->a[i], val, sizeof(token_lex));
		}
	}
}


void set_val_cap_token_lex(vector_token_lex* vec, token_lex* val)
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
			memcpy(&vec->a[i], val, sizeof(token_lex));
		}
	}
}


void clear_token_lex(vector_token_lex* vec)
{
	size_t i;
	if (vec->elem_free) {
		for (i=0; i<vec->size; ++i) {
			vec->elem_free(&vec->a[i]);
		}
	}
	vec->size = 0;
}


void free_vec_token_lex_heap(void* vec)
{
	size_t i;
	vector_token_lex* tmp = vec;
	if (tmp->elem_free) {
		for (i=0; i<tmp->size; i++) {
			tmp->elem_free(&tmp->a[i]);
		}
	}
	free(tmp->a);
	free(tmp);
}


void free_vec_token_lex(void* vec)
{
	size_t i;
	vector_token_lex* tmp = vec;
	if (tmp->elem_free) {
		for (i=0; i<tmp->size; i++) {
			tmp->elem_free(&tmp->a[i]);
		}
	}

	free(tmp->a);

	tmp->size = 0;
	tmp->capacity = 0;
}




