#include "vector_char.h"

#include <assert.h>

#define STDERR(X) fprintf(stderr, X)

#define VEC_char_ALLOCATOR(x) ((x) * 2)


size_t VECTOR_char_SZ = 50;



vector_char* vec_char_heap(size_t size, size_t capacity)
{
	vector_char* vec;
	if (!(vec = malloc(sizeof(vector_char)))) {
		assert(vec != NULL);
		return NULL;
	}

	vec->size = (size > 0) ? size : 0;
	vec->capacity = (capacity > vec->size || (vec->size && capacity == vec->size)) ? capacity : vec->size + VECTOR_char_SZ;

	if (!(vec->a = malloc(vec->capacity*sizeof(char)))) {
		assert(vec->a != NULL);
		free(vec);
		return NULL;
	}

	return vec;
}


vector_char* init_vec_char_heap(char* vals, size_t num)
{
	vector_char* vec;
	
	if (!(vec = malloc(sizeof(vector_char)))) {
		assert(vec != NULL);
		return NULL;
	}

	vec->capacity = num + VECTOR_char_SZ;
	vec->size = num;
	if (!(vec->a = malloc(vec->capacity*sizeof(char)))) {
		assert(vec->a != NULL);
		free(vec);
		return NULL;
	}

	memcpy(vec->a, vals, sizeof(char)*num);

	return vec;
}


int vec_char(vector_char* vec, size_t size, size_t capacity)
{
	vec->size = (size > 0) ? size : 0;
	vec->capacity = (capacity > vec->size || (vec->size && capacity == vec->size)) ? capacity : vec->size + VECTOR_char_SZ;

	if (!(vec->a = malloc(vec->capacity*sizeof(char)))) {
		assert(vec->a != NULL);
		vec->size = vec->capacity = 0;
		return 0;
	}

	return 1;
}


int init_vec_char(vector_char* vec, char* vals, size_t num)
{
	vec->capacity = num + VECTOR_char_SZ;
	vec->size = num;
	if (!(vec->a = malloc(vec->capacity*sizeof(char)))) {
		assert(vec->a != NULL);
		vec->size = vec->capacity = 0;
		return 0;
	}

	memcpy(vec->a, vals, sizeof(char)*num);

	return 1;
}

void vec_char_copy(void* dest, void* src)
{
	vector_char* vec1 = dest;
	vector_char* vec2 = src;
	
	vec1->size = 0;
	vec1->capacity = 0;
	
	/*not much else we can do here*/
	if (!(vec1->a = malloc(vec2->capacity*sizeof(char)))) {
		assert(vec1->a != NULL);
		return;
	}
	
	memcpy(vec1->a, vec2->a, vec2->size*sizeof(char));
	vec1->size = vec2->size;
	vec1->capacity = vec2->capacity;
}





int push_char(vector_char* vec, char a)
{
	void* tmp;
	size_t tmp_sz;
	if (vec->capacity > vec->size) {
		vec->a[vec->size++] = a;
	} else {
		tmp_sz = VEC_char_ALLOCATOR(vec->capacity);
		if (!(tmp = realloc(vec->a, sizeof(char)*tmp_sz))) {
			assert(tmp != NULL);
			return 0;
		}
		vec->a = tmp;
		vec->a[vec->size++] = a;
		vec->capacity = tmp_sz;
	}
	return 1;
}



char pop_char(vector_char* vec)
{
	return vec->a[--vec->size];
}

char* back_char(vector_char* vec)
{
	return &vec->a[vec->size-1];
}


int extend_char(vector_char* vec, size_t num)
{
	void* tmp;
	size_t tmp_sz;
	if (vec->capacity < vec->size + num) {
		tmp_sz = vec->capacity + num + VECTOR_char_SZ;
		if (!(tmp = realloc(vec->a, sizeof(char)*tmp_sz))) {
			assert(tmp != NULL);
			return 0;
		}
		vec->a = tmp;
		vec->capacity = tmp_sz;
	}

	vec->size += num;
	return 1;
}


int insert_char(vector_char* vec, size_t i, char a)
{
	void* tmp;
	size_t tmp_sz;
	if (vec->capacity > vec->size) {
		memmove(&vec->a[i+1], &vec->a[i], (vec->size-i)*sizeof(char));
		vec->a[i] = a;
	} else {
		tmp_sz = VEC_char_ALLOCATOR(vec->capacity);
		if (!(tmp = realloc(vec->a, sizeof(char)*tmp_sz))) {
			assert(tmp != NULL);
			return 0;
		}
		vec->a = tmp;
		memmove(&vec->a[i+1], &vec->a[i], (vec->size-i)*sizeof(char));
		vec->a[i] = a;
		vec->capacity = tmp_sz;
	}

	vec->size++;
	return 1;
}


int insert_array_char(vector_char* vec, size_t i, char* a, size_t num)
{
	void* tmp;
	size_t tmp_sz;
	if (vec->capacity < vec->size + num) {
		tmp_sz = vec->capacity + num + VECTOR_char_SZ;
		if (!(tmp = realloc(vec->a, sizeof(char)*tmp_sz))) {
			assert(tmp != NULL);
			return 0;
		}
		vec->a = tmp;
		vec->capacity = tmp_sz;
	}

	memmove(&vec->a[i+num], &vec->a[i], (vec->size-i)*sizeof(char));
	memcpy(&vec->a[i], a, num*sizeof(char));
	vec->size += num;
	return 1;
}


void erase_char(vector_char* vec, size_t start, size_t end)
{
	size_t d = end - start + 1;
	memmove(&vec->a[start], &vec->a[end+1], (vec->size-1-end)*sizeof(char));
	vec->size -= d;
}


int reserve_char(vector_char* vec, size_t size)
{
	void* tmp;
	if (vec->capacity < size) {
		if (!(tmp = realloc(vec->a, sizeof(char)*(size+VECTOR_char_SZ)))) {
			assert(tmp != NULL);
			return 0;
		}
		vec->a = tmp;
		vec->capacity = size + VECTOR_char_SZ;
	}
	return 1;
}




int set_capacity_char(vector_char* vec, size_t size)
{
	void* tmp;
	if (size < vec->size) {
		vec->size = size;
	}

	if (!(tmp = realloc(vec->a, sizeof(char)*size))) {
		assert(tmp != NULL);
		return 0;
	}
	vec->a = tmp;
	vec->capacity = size;
	return 1;
}



void set_val_sz_char(vector_char* vec, char val)
{
	size_t i;
	for (i=0; i<vec->size; i++) {
		vec->a[i] = val;
	}
}


void set_val_cap_char(vector_char* vec, char val)
{
	size_t i;
	for (i=0; i<vec->capacity; i++) {
		vec->a[i] = val;
	}
}


void clear_char(vector_char* vec) { vec->size = 0; }

void free_vec_char_heap(void* vec)
{
	vector_char* tmp = vec;
	free(tmp->a);
	free(tmp);
}

void free_vec_char(void* vec)
{
	vector_char* tmp = vec;
	free(tmp->a);
	tmp->size = 0;
	tmp->capacity = 0;
}
