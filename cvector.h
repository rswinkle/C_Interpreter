/* header starts */

#ifndef CVECTOR_H
#define CVECTOR_H

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <assert.h>

#ifdef __cplusplus
extern "C" {
#endif

/** Data structure for int vector. */
typedef struct vector_i
{
	int* a;            /**< Array. */
	size_t size;       /**< Current size (amount you use when manipulating array directly). */
	size_t capacity;   /**< Allocated size of array; always >= size. */
} vector_i;

extern size_t VEC_I_START_SZ;

int vec_i(vector_i* vec, size_t size, size_t capacity);
int init_vec_i(vector_i* vec, int* vals, size_t num);

vector_i* vec_i_heap(size_t size, size_t capacity);
vector_i* init_vec_i_heap(int* vals, size_t num);
void vec_i_copy(void* dest, void* src);

int push_i(vector_i* vec, int a);
int pop_i(vector_i* vec);

int extend_i(vector_i* vec, size_t num);
int insert_i(vector_i* vec, size_t i, int a);
int insert_array_i(vector_i* vec, size_t i, int* a, size_t num);
void erase_i(vector_i* vec, size_t start, size_t end);
int reserve_i(vector_i* vec, size_t size);
int set_capacity_i(vector_i* vec, size_t size);
void set_val_sz_i(vector_i* vec, int val);
void set_val_cap_i(vector_i* vec, int val);

int* back_i(vector_i* vec);

void clear_i(vector_i* vec);
void free_vec_i_heap(void* vec);
void free_vec_i(void* vec);

/** Data structure for double vector. */
typedef struct vector_d
{
	double* a;         /**< Array. */
	size_t size;       /**< Current size (amount you use when manipulating array directly). */
	size_t capacity;   /**< Allocated size of array; always >= size. */
} vector_d;

extern size_t VEC_D_START_SZ;

int vec_d(vector_d* vec, size_t size, size_t capacity);
int init_vec_d(vector_d* vec, double* vals, size_t num);

vector_d* vec_d_heap(size_t size, size_t capacity);
vector_d* init_vec_d_heap(double* vals, size_t num);

void vec_d_copy(void* dest, void* src);

int push_d(vector_d* vec, double a);
double pop_d(vector_d* vec);

int extend_d(vector_d* vec, size_t num);
int insert_d(vector_d* vec, size_t i, double a);
int insert_array_d(vector_d* vec, size_t i, double* a, size_t num);
void erase_d(vector_d* vec, size_t start, size_t end);
int reserve_d(vector_d* vec, size_t size);
int set_capacity_d(vector_d* vec, size_t size);
void set_val_sz_d(vector_d* vec, double val);
void set_val_cap_d(vector_d* vec, double val);

double* back_d(vector_d* vec);

void clear_d(vector_d* vec);
void free_vec_d_heap(void* vec);
void free_vec_d(void* vec);

/** Data structure for string vector. */
typedef struct vector_str
{
	char** a;          /**< Array. */
	size_t size;       /**< Current size (amount you use when manipulating array directly). */
	size_t capacity;   /**< Allocated size of array; always >= size. */
} vector_str;

extern size_t VEC_STR_START_SZ;

//char* mystrdup(const char* str);

int vec_str(vector_str* vec, size_t size, size_t capacity);
int init_vec_str(vector_str* vec, char** vals, size_t num);

vector_str* vec_str_heap(size_t size, size_t capacity);
vector_str* init_vec_str_heap(char** vals, size_t num);

void vec_str_copy(void* dest, void* src);

int push_str(vector_str* vec, char* a);
void pop_str(vector_str* vec, char* ret);

int extend_str(vector_str* vec, size_t num);
int insert_str(vector_str* vec, size_t i, char* a);
int insert_array_str(vector_str* vec, size_t i, char** , size_t num);
void erase_str(vector_str* vec, size_t start, size_t end);
int reserve_str(vector_str* vec, size_t size);
int set_capacity_str(vector_str* vec, size_t size);
void set_val_sz_str(vector_str* vec, char* val);
void set_val_cap_str(vector_str* vec, char* val);

char** back_str(vector_str* vec);

void clear_str(vector_str* vec);
void free_vec_str_heap(void* vec);
void free_vec_str(void* vec);

typedef unsigned char u8;

/** Data structure for generic type (cast to void) vectors */
typedef struct vector_void
{
	u8* a;                 /**< Array. */
	size_t size;             /**< Current size (amount you should use when manipulating array directly). */
	size_t capacity;         /**< Allocated size of array; always >= size. */
	size_t elem_size;        /**< Size in u8s of type stored (sizeof(T) where T is type). */
	void (*elem_init)(void*, void*);
	void (*elem_free)(void*);
} vector_void;

extern size_t VEC_VOID_START_SZ;

#define GET_VOID(VEC, TYPE, I) ((TYPE*)&(VEC)->a[(I)*(VEC)->elem_size])

int vec_void(vector_void* vec, size_t size, size_t capacity, size_t elem_sz, void(*elem_free)(void*), void(*elem_init)(void*, void*));
int init_vec_void(vector_void* vec, void* vals, size_t num, size_t elem_sz, void(*elem_free)(void*), void(*elem_init)(void*, void*));

vector_void* vec_void_heap(size_t size, size_t capacity, size_t elem_sz, void (*elem_free)(void*), void(*elem_init)(void*, void*));
vector_void* init_vec_void_heap(void* vals, size_t num, size_t elem_sz, void (*elem_free)(void*), void(*elem_init)(void*, void*));

void vec_void_copy(void* dest, void* src);

int push_void(vector_void* vec, void* val);
void pop_void(vector_void* vec, void* ret);
void* vec_void_get(vector_void* vec, size_t i);

int extend_void(vector_void* vec, size_t num);
int insert_void(vector_void* vec, size_t i, void* a);
int insert_array_void(vector_void* vec, size_t i, void* a, size_t num);
void erase_void(vector_void* vec, size_t start, size_t end);
int reserve_void(vector_void* vec, size_t size);
int set_capacity_void(vector_void* vec, size_t size);
void set_val_sz_void(vector_void* vec, void* val);
void set_val_cap_void(vector_void* vec, void* val);

void* back_void(vector_void* vec);

void clear_void(vector_void* vec);
void free_vec_void_heap(void* vec);
void free_vec_void(void* vec);

#ifdef __cplusplus
}
#endif

/* header ends */
#endif 


#ifdef CVECTOR_IMPLEMENTATION

size_t VEC_I_START_SZ = 50;

#define VEC_I_ALLOCATOR(x) ((x) * 2)

vector_i* vec_i_heap(size_t size, size_t capacity)
{
	vector_i* vec;
	if (!(vec = (vector_i*)malloc(sizeof(vector_i)))) {
		assert(vec != NULL);
		return NULL;
	}

	vec->size = (size > 0) ? size : 0;
	vec->capacity = (capacity > vec->size || (vec->size && capacity == vec->size)) ? capacity : vec->size + VEC_I_START_SZ;

	if (!(vec->a = (int*)malloc(vec->capacity*sizeof(int)))) {
		assert(vec->a != NULL);
		free(vec);
		return NULL;
	}

	return vec;
}

vector_i* init_vec_i_heap(int* vals, size_t num)
{
	vector_i* vec;
	
	if (!(vec = (vector_i*)malloc(sizeof(vector_i)))) {
		assert(vec != NULL);
		return NULL;
	}

	vec->capacity = num + VEC_I_START_SZ;
	vec->size = num;
	if (!(vec->a = (int*)malloc(vec->capacity*sizeof(int)))) {
		assert(vec->a != NULL);
		free(vec);
		return NULL;
	}

	memcpy(vec->a, vals, sizeof(int)*num);

	return vec;
}

int vec_i(vector_i* vec, size_t size, size_t capacity)
{
	vec->size = (size > 0) ? size : 0;
	vec->capacity = (capacity > vec->size || (vec->size && capacity == vec->size)) ? capacity : vec->size + VEC_I_START_SZ;

	if (!(vec->a = (int*)malloc(vec->capacity*sizeof(int)))) {
		assert(vec->a != NULL);
		vec->size = vec->capacity = 0;
		return 0;
	}

	return 1;
}

int init_vec_i(vector_i* vec, int* vals, size_t num)
{
	vec->capacity = num + VEC_I_START_SZ;
	vec->size = num;
	if (!(vec->a = (int*)malloc(vec->capacity*sizeof(int)))) {
		assert(vec->a != NULL);
		vec->size = vec->capacity = 0;
		return 0;
	}

	memcpy(vec->a, vals, sizeof(int)*num);

	return 1;
}

void vec_i_copy(void* dest, void* src)
{
	vector_i* vec1 = (vector_i*)dest;
	vector_i* vec2 = (vector_i*)src;
	
	vec1->size = 0;
	vec1->capacity = 0;
	
	
	if (!(vec1->a = (int*)malloc(vec2->capacity*sizeof(int)))) {
		assert(vec1->a != NULL);
		return;
	}
	
	memcpy(vec1->a, vec2->a, vec2->size*sizeof(int));
	vec1->size = vec2->size;
	vec1->capacity = vec2->capacity;
}

int push_i(vector_i* vec, int a)
{
	int* tmp;
	size_t tmp_sz;
	if (vec->capacity == vec->size) {
		tmp_sz = VEC_I_ALLOCATOR(vec->capacity);
		if (!(tmp = (int*)realloc(vec->a, sizeof(int)*tmp_sz))) {
			assert(tmp != NULL);
			return 0;
		}
		vec->a = tmp;
		vec->capacity = tmp_sz;
	}
	
	vec->a[vec->size++] = a;
	return 1;
}

int pop_i(vector_i* vec)
{
	return vec->a[--vec->size];
}

int* back_i(vector_i* vec)
{
	return &vec->a[vec->size-1];
}

int extend_i(vector_i* vec, size_t num)
{
	int* tmp;
	size_t tmp_sz;
	if (vec->capacity < vec->size + num) {
		tmp_sz = vec->capacity + num + VEC_I_START_SZ;
		if (!(tmp = (int*)realloc(vec->a, sizeof(int)*tmp_sz))) {
			assert(tmp != NULL);
			return 0;
		}
		vec->a = tmp;
		vec->capacity = tmp_sz;
	}

	vec->size += num;
	return 1;
}

int insert_i(vector_i* vec, size_t i, int a)
{
	int* tmp;
	size_t tmp_sz;
	if (vec->capacity == vec->size) {
		tmp_sz = VEC_I_ALLOCATOR(vec->capacity);
		if (!(tmp = (int*)realloc(vec->a, sizeof(int)*tmp_sz))) {
			assert(tmp != NULL);
			return 0;
		}
		vec->a = tmp;
		vec->capacity = tmp_sz;
	}

	memmove(&vec->a[i+1], &vec->a[i], (vec->size-i)*sizeof(int));
	vec->a[i] = a;
	vec->size++;
	return 1;
}

int insert_array_i(vector_i* vec, size_t i, int* a, size_t num)
{
	int* tmp;
	size_t tmp_sz;
	if (vec->capacity < vec->size + num) {
		tmp_sz = vec->capacity + num + VEC_I_START_SZ;
		if (!(tmp = (int*)realloc(vec->a, sizeof(int)*tmp_sz))) {
			assert(tmp != NULL);
			return 0;
		}
		vec->a = tmp;
		vec->capacity = tmp_sz;
	}

	memmove(&vec->a[i+num], &vec->a[i], (vec->size-i)*sizeof(int));
	memcpy(&vec->a[i], a, num*sizeof(int));
	vec->size += num;
	return 1;
}

void erase_i(vector_i* vec, size_t start, size_t end)
{
	size_t d = end - start + 1;
	memmove(&vec->a[start], &vec->a[end+1], (vec->size-1-end)*sizeof(int));
	vec->size -= d;
}

int reserve_i(vector_i* vec, size_t size)
{
	int* tmp;
	if (vec->capacity < size) {
		if (!(tmp = (int*)realloc(vec->a, sizeof(int)*(size+VEC_I_START_SZ)))) {
			assert(tmp != NULL);
			return 0;
		}
		vec->a = tmp;
		vec->capacity = size + VEC_I_START_SZ;
	}
	return 1;
}

int set_capacity_i(vector_i* vec, size_t size)
{
	int* tmp;
	if (size < vec->size) {
		vec->size = size;
	}

	if (!(tmp = (int*)realloc(vec->a, sizeof(int)*size))) {
		assert(tmp != NULL);
		return 0;
	}
	vec->a = tmp;
	vec->capacity = size;
	return 1;
}

void set_val_sz_i(vector_i* vec, int val)
{
	size_t i;
	for (i=0; i<vec->size; i++) {
		vec->a[i] = val;
	}
}

void set_val_cap_i(vector_i* vec, int val)
{
	size_t i;
	for (i=0; i<vec->capacity; i++) {
		vec->a[i] = val;
	}
}

void clear_i(vector_i* vec) { vec->size = 0; }

void free_vec_i_heap(void* vec)
{
	vector_i* tmp = (vector_i*)vec;
	free(tmp->a);
	free(tmp);
}

void free_vec_i(void* vec)
{
	vector_i* tmp = (vector_i*)vec;
	free(tmp->a);
	tmp->size = 0;
	tmp->capacity = 0;
}

size_t VEC_D_START_SZ = 50;

#define VEC_D_ALLOCATOR(x) ((x) * 2)

vector_d* vec_d_heap(size_t size, size_t capacity)
{
	vector_d* vec;
	
	if (!(vec = (vector_d*)malloc(sizeof(vector_d)))) {
		assert(vec != NULL);
		return NULL;
	}

	vec->size = (size > 0) ? size : 0;
	vec->capacity = (capacity > vec->size || (vec->size && capacity == vec->size)) ? capacity : vec->size + VEC_D_START_SZ;

	if (!(vec->a = (double*)malloc(vec->capacity*sizeof(double)))) {
		assert(vec->a != NULL);
		free(vec);
		return NULL;
	}

	return vec;
}

vector_d* init_vec_d_heap(double* vals, size_t num)
{
	vector_d* vec;
	
	if (!(vec = (vector_d*)malloc(sizeof(vector_d)))) {
		assert(vec != NULL);
		return NULL;
	}

	vec->capacity = num + VEC_D_START_SZ;
	vec->size = num;
	if (!(vec->a = (double*)malloc(vec->capacity*sizeof(double)))) {
		assert(vec->a != NULL);
		free(vec);
		return NULL;
	}

	memcpy(vec->a, vals, sizeof(double)*num);

	return vec;
}

int vec_d(vector_d* vec, size_t size, size_t capacity)
{
	vec->size = (size > 0) ? size : 0;
	vec->capacity = (capacity > vec->size || (vec->size && capacity == vec->size)) ? capacity : vec->size + VEC_D_START_SZ;

	if (!(vec->a = (double*)malloc(vec->capacity*sizeof(double)))) {
		assert(vec->a != NULL);
		vec->size = vec->capacity = 0;
		return 0;
	}

	return 1;
}

int init_vec_d(vector_d* vec, double* vals, size_t num)
{
	vec->capacity = num + VEC_D_START_SZ;
	vec->size = num;
	if (!(vec->a = (double*)malloc(vec->capacity*sizeof(double)))) {
		assert(vec->a != NULL);
		vec->size = vec->capacity = 0;
		return 0;
	}

	memcpy(vec->a, vals, sizeof(double)*num);

	return 1;
}

void vec_d_copy(void* dest, void* src)
{
	vector_d* vec1 = (vector_d*)dest;
	vector_d* vec2 = (vector_d*)src;
	
	vec1->size = 0;
	vec1->capacity = 0;
	
	
	if (!(vec1->a = (double*)malloc(vec2->capacity*sizeof(double)))) {
		assert(vec1->a != NULL);
		return;
	}
	
	memcpy(vec1->a, vec2->a, vec2->size*sizeof(double));
	vec1->size = vec2->size;
	vec1->capacity = vec2->capacity;
}

int push_d(vector_d* vec, double a)
{
	double* tmp;
	size_t tmp_sz;
	if (vec->capacity == vec->size) {
		tmp_sz = VEC_D_ALLOCATOR(vec->capacity);
		if (!(tmp = (double*)realloc(vec->a, sizeof(double)*tmp_sz))) {
			assert(tmp != NULL);
			return 0;
		}
		vec->a = tmp;
		vec->capacity = tmp_sz;
	}
	vec->a[vec->size++] = a;
	return 1;
}

double pop_d(vector_d* vec)
{
	return vec->a[--vec->size];
}

double* back_d(vector_d* vec)
{
	return &vec->a[vec->size-1];
}

int extend_d(vector_d* vec, size_t num)
{
	double* tmp;
	size_t tmp_sz;
	if (vec->capacity < vec->size + num) {
		tmp_sz = vec->capacity + num + VEC_D_START_SZ;
		if (!(tmp = (double*)realloc(vec->a, sizeof(double)*tmp_sz))) {
			assert(tmp != NULL);
			return 0;
		}
		vec->a = tmp;
		vec->capacity = tmp_sz;
	}

	vec->size += num;
	return 1;
}

int insert_d(vector_d* vec, size_t i, double a)
{
	double* tmp;
	size_t tmp_sz;
	if (vec->capacity == vec->size) {
		tmp_sz = VEC_D_ALLOCATOR(vec->capacity);
		if (!(tmp = (double*)realloc(vec->a, sizeof(double)*tmp_sz))) {
			assert(tmp != NULL);
			return 0;
		}
		vec->a = tmp;
		vec->capacity = tmp_sz;
	}
	
	memmove(&vec->a[i+1], &vec->a[i], (vec->size-i)*sizeof(double));
	vec->a[i] = a;
	vec->size++;
	return 1;
}

int insert_array_d(vector_d* vec, size_t i, double* a, size_t num)
{
	double* tmp;
	size_t tmp_sz;
	if (vec->capacity < vec->size + num) {
		tmp_sz = vec->capacity + num + VEC_D_START_SZ;
		if (!(tmp = (double*)realloc(vec->a, sizeof(double)*tmp_sz))) {
			assert(tmp != NULL);
			return 0;
		}
		vec->a = tmp;
		vec->capacity = tmp_sz;
	}

	memmove(&vec->a[i+num], &vec->a[i], (vec->size-i)*sizeof(double));
	memcpy(&vec->a[i], a, num*sizeof(double));
	vec->size += num;
	return 1;
}

void erase_d(vector_d* vec, size_t start, size_t end)
{
	size_t d = end - start + 1;
	memmove(&vec->a[start], &vec->a[end+1], (vec->size-1-end)*sizeof(double));
	vec->size -= d;
}

int reserve_d(vector_d* vec, size_t size)
{
	double* tmp;
	if (vec->capacity < size) {
		if (!(tmp = (double*)realloc(vec->a, sizeof(double)*(size+VEC_D_START_SZ)))) {
			assert(tmp != NULL);
			return 0;
		}
		vec->a = tmp;
		vec->capacity = size + VEC_D_START_SZ;
	}
	return 1;
}

int set_capacity_d(vector_d* vec, size_t size)
{
	double* tmp;
	if (size < vec->size)
		vec->size = size;

	if (!(tmp = (double*)realloc(vec->a, sizeof(double)*size))) {
		assert(tmp != NULL);
		return 0;
	}
	vec->a = tmp;
	vec->capacity = size;
	return 1;
}

void set_val_sz_d(vector_d* vec, double val)
{
	size_t i;
	for(i=0; i<vec->size; i++) {
		vec->a[i] = val;
	}
}

void set_val_cap_d(vector_d* vec, double val)
{
	size_t i;
	for(i=0; i<vec->capacity; i++) {
		vec->a[i] = val;
	}
}

void clear_d(vector_d* vec) { vec->size = 0; }

void free_vec_d_heap(void* vec)
{
	vector_d* tmp = (vector_d*)vec;
	free(tmp->a);
	free(tmp);
}

void free_vec_d(void* vec)
{
	vector_d* tmp = (vector_d*)vec;
	free(tmp->a);
	tmp->size = 0;
	tmp->capacity = 0;
}

size_t VEC_STR_START_SZ = 20;

#define VEC_STR_ALLOCATOR(x) ((x) * 2)
/*
char* mystrdup(const char* str)
{
	
	size_t len = strlen(str);
	char* temp = (char*)calloc(len+1, sizeof(char));
	if (!temp) {
		assert(temp != NULL);
		return NULL;
	}
	
	return (char*)memcpy(temp, str, len);  
}
*/

vector_str* vec_str_heap(size_t size, size_t capacity)
{
	vector_str* vec;
	if (!(vec = (vector_str*)malloc(sizeof(vector_str)))) {
		assert(vec != NULL);
		return NULL;
	}

	vec->size = (size > 0) ? size : 0;
	vec->capacity = (capacity > vec->size || (vec->size && capacity == vec->size)) ? capacity : vec->size + VEC_STR_START_SZ;

	
	if (!(vec->a = (char**)calloc(vec->capacity, sizeof(char*)))) {
		assert(vec->a != NULL);
		free(vec);
		return NULL;
	}

	return vec;
}

vector_str* init_vec_str_heap(char** vals, size_t num)
{
	vector_str* vec;
	size_t i;
	
	if (!(vec = (vector_str*)malloc(sizeof(vector_str)))) {
		assert(vec != NULL);
		return NULL;
	}

	vec->capacity = num + VEC_STR_START_SZ;
	vec->size = num;
	if (!(vec->a = (char**)malloc(vec->capacity*sizeof(char*)))) {
		assert(vec->a != NULL);
		free(vec);
		return NULL;
	}

	for(i=0; i<num; i++) {
		vec->a[i] = mystrdup(vals[i]);
	}
	
	return vec;
}

int vec_str(vector_str* vec, size_t size, size_t capacity)
{
	vec->size = (size > 0) ? size : 0;
	vec->capacity = (capacity > vec->size || (vec->size && capacity == vec->size)) ? capacity : vec->size + VEC_STR_START_SZ;

	
	if (!(vec->a = (char**)calloc(vec->capacity, sizeof(char*)))) {
		assert(vec->a != NULL);
		vec->size = vec->capacity = 0;
		return 0;
	}

	return 1;
}

int init_vec_str(vector_str* vec, char** vals, size_t num)
{
	size_t i;
	
	vec->capacity = num + VEC_STR_START_SZ;
	vec->size = num;
	if (!(vec->a = (char**)malloc(vec->capacity*sizeof(char*)))) {
		assert(vec->a != NULL);
		vec->size = vec->capacity = 0;
		return 0;
	}

	for(i=0; i<num; i++) {
		vec->a[i] = mystrdup(vals[i]);
	}
	
	return 1;
}

void vec_str_copy(void* dest, void* src)
{
	size_t i;
	vector_str* vec1 = (vector_str*)dest;
	vector_str* vec2 = (vector_str*)src;
	
	vec1->size = 0;
	vec1->capacity = 0;
	
	
	if (!(vec1->a = (char**)malloc(vec2->capacity*sizeof(char*)))) {
		assert(vec1->a != NULL);
		return;
	}
	
	for (i=0; i<vec2->size; ++i) {
		vec1->a[i] = mystrdup(vec2->a[i]);
	}
	
	vec1->size = vec2->size;
	vec1->capacity = vec2->capacity;
}

int push_str(vector_str* vec, char* a)
{
	char** tmp;
	size_t tmp_sz;
	if (vec->capacity == vec->size) {
		tmp_sz = VEC_STR_ALLOCATOR(vec->capacity);
		if (!(tmp = (char**)realloc(vec->a, sizeof(char*)*tmp_sz))) {
			assert(tmp != NULL);
			return 0;
		}
		vec->a = tmp;
		vec->capacity = tmp_sz;
	}
	
	vec->a[vec->size++] = mystrdup(a);
	return 1;
}

void pop_str(vector_str* vec, char* ret)
{
	if (ret) {
		strcpy(ret, vec->a[--vec->size]);
	}
	free(vec->a[vec->size]);
}

char** back_str(vector_str* vec)
{
	return &vec->a[vec->size-1];
}

int extend_str(vector_str* vec, size_t num)
{
	char** tmp;
	size_t tmp_sz;
	if (vec->capacity < vec->size + num) {
		tmp_sz = vec->capacity + num + VEC_STR_START_SZ;
		if (!(tmp = (char**)realloc(vec->a, sizeof(char*)*tmp_sz))) {
			assert(tmp != NULL);
			return 0;
		}
		vec->a = tmp;
		vec->capacity = tmp_sz;
	}

	memset(&vec->a[vec->size], 0, num*sizeof(char*));
	vec->size += num;
	return 1;
}

int insert_str(vector_str* vec, size_t i, char* a)
{
	char** tmp;
	size_t tmp_sz;
	if (vec->capacity == vec->size) {
		tmp_sz = VEC_STR_ALLOCATOR(vec->capacity);
		if (!(tmp = (char**)realloc(vec->a, sizeof(char*)*tmp_sz))) {
			assert(tmp != NULL);
			return 0;
		}
		vec->a = tmp;
		vec->capacity = tmp_sz;
	}

	memmove(&vec->a[i+1], &vec->a[i], (vec->size-i)*sizeof(char*));
	vec->a[i] = mystrdup(a);
	vec->size++;
	return 1;
}

int insert_array_str(vector_str* vec, size_t i, char** a, size_t num)
{
	char** tmp;
	size_t tmp_sz, j;
	if (vec->capacity < vec->size + num) {
		tmp_sz = vec->capacity + num + VEC_STR_START_SZ;
		if (!(tmp = (char**)realloc(vec->a, sizeof(char*)*tmp_sz))) {
			assert(tmp != NULL);
			return 0;
		}
		vec->a = tmp;
		vec->capacity = tmp_sz;
	}

	memmove(&vec->a[i+num], &vec->a[i], (vec->size-i)*sizeof(char*));
	for (j=0; j<num; ++j) {
		vec->a[j+i] = mystrdup(a[j]);
	}
	
	vec->size += num;
	return 1;
}

void erase_str(vector_str* vec, size_t start, size_t end)
{
	size_t i;
	size_t d = end - start + 1;
	for (i=start; i<=end; i++) {
		free(vec->a[i]);
	}
	
	memmove(&vec->a[start], &vec->a[end+1], (vec->size-1-end)*sizeof(char*));
	vec->size -= d;
}

int reserve_str(vector_str* vec, size_t size)
{
	char** tmp;
	if (vec->capacity < size) {
		if (!(tmp = (char**)realloc(vec->a, sizeof(char*)*(size+VEC_STR_START_SZ)))) {
			assert(tmp != NULL);
			return 0;
		}
		vec->a = tmp;
		vec->capacity = size + VEC_STR_START_SZ;
	}
	return 1;
}

int set_capacity_str(vector_str* vec, size_t size)
{
	size_t i;
	char** tmp;
	if (size < vec->size) {
		for(i=vec->size-1; i>size-1; i--) {
			free(vec->a[i]);
		}

		vec->size = size;
	}

	if (!(tmp = (char**)realloc(vec->a, sizeof(char*)*size))) {
		assert(tmp != NULL);
		return 0;
	}
	vec->a = tmp;
	vec->capacity = size;
	return 1;
}

void set_val_sz_str(vector_str* vec, char* val)
{
	size_t i;
	for(i=0; i<vec->size; i++) {
		free(vec->a[i]);

		
		vec->a[i] = mystrdup(val);
	}
}

void set_val_cap_str(vector_str* vec, char* val)
{
	size_t i;
	for (i=0; i<vec->capacity; i++) {
		if (i<vec->size) {
			free(vec->a[i]);
		}
		
		vec->a[i] = mystrdup(val);
	}
	vec->size = vec->capacity;
}

void clear_str(vector_str* vec)
{
	int i;
	for (i=0; i<vec->size; i++) {
		free(vec->a[i]);
	}
	
	vec->size = 0;
}

void free_vec_str_heap(void* vec)
{
	size_t i;
	vector_str* tmp = (vector_str*)vec;
	for (i=0; i<tmp->size; i++) {
		free(tmp->a[i]);
	}
	
	free(tmp->a);
	free(tmp);
}

void free_vec_str(void* vec)
{
	size_t i;
	vector_str* tmp = (vector_str*)vec;
	for (i=0; i<tmp->size; i++) {
		free(tmp->a[i]);
	}
	
	free(tmp->a);
	tmp->size = 0;
	tmp->capacity = 0;
}

size_t VEC_VOID_START_SZ = 20;

#define VEC_VOID_ALLOCATOR(x) ((x) * 2)

vector_void* vec_void_heap(size_t size, size_t capacity, size_t elem_sz, void(*elem_free)(void*), void(*elem_init)(void*, void*))
{
	vector_void* vec;
	if (!(vec = (vector_void*)malloc(sizeof(vector_void)))) {
		assert(vec != NULL);
		return NULL;
	}

	vec->size = (size > 0) ? size : 0;
	vec->capacity = (capacity > vec->size || (vec->size && capacity == vec->size)) ? capacity : vec->size + VEC_VOID_START_SZ;

	vec->elem_size = elem_sz;
	
	
	if (!(vec->a = (byte*)malloc(vec->capacity*elem_sz))) {
		assert(vec->a != NULL);
		free(vec);
		return NULL;
	}

	vec->elem_free = elem_free;
	vec->elem_init = elem_init;

	return vec;
}

vector_void* init_vec_void_heap(void* vals, size_t num, size_t elem_sz, void(*elem_free)(void*), void(*elem_init)(void*, void*))
{
	vector_void* vec;
	size_t i;
	
	if (!(vec = (vector_void*)malloc(sizeof(vector_void)))) {
		assert(vec != NULL);
		return NULL;
	}

	vec->elem_size = elem_sz;

	vec->capacity = num + VEC_VOID_START_SZ;
	vec->size = num;
	if (!(vec->a = (byte*)malloc(vec->capacity*elem_sz))) {
		assert(vec->a != NULL);
		free(vec);
		return NULL;
	}

	if (elem_init) {
		for (i=0; i<num; ++i) {
			elem_init(&vec->a[i*elem_sz], &((byte*)vals)[i*elem_sz]);
		}
	} else {
		memcpy(vec->a, vals, elem_sz*num);
	}
	
	vec->elem_free = elem_free;
	vec->elem_init = elem_init;

	return vec;
}

int vec_void(vector_void* vec, size_t size, size_t capacity, size_t elem_sz, void(*elem_free)(void*), void(*elem_init)(void*, void*))
{
	vec->size = (size > 0) ? size : 0;
	vec->capacity = (capacity > vec->size || (vec->size && capacity == vec->size)) ? capacity : vec->size + VEC_VOID_START_SZ;

	vec->elem_size = elem_sz;
	
	if (!(vec->a = (byte*)malloc(vec->capacity*elem_sz))) {
		assert(vec->a != NULL);
		vec->size = vec->capacity = 0;
		return 0;
	}

	vec->elem_free = elem_free;
	vec->elem_init = elem_init;

	return 1;
}

int init_vec_void(vector_void* vec, void* vals, size_t num, size_t elem_sz, void(*elem_free)(void*), void(*elem_init)(void*, void*))
{
	size_t i;
	
	vec->elem_size = elem_sz;

	vec->capacity = num + VEC_VOID_START_SZ;
	vec->size = num;
	if (!(vec->a = (byte*)malloc(vec->capacity*elem_sz))) {
		assert(vec->a != NULL);
		vec->size = vec->capacity = 0;
		return 0;
	}

	if (elem_init) {
		for (i=0; i<num; ++i) {
			elem_init(&vec->a[i*elem_sz], &((byte*)vals)[i*elem_sz]);
		}
	} else {
		memcpy(vec->a, vals, elem_sz*num);
	}

	vec->elem_free = elem_free;
	vec->elem_init = elem_init;

	return 1;
}

void vec_void_copy(void* dest, void* src)
{
	size_t i;
	vector_void* vec1 = (vector_void*)dest;
	vector_void* vec2 = (vector_void*)src;
	
	vec1->size = 0;
	vec1->capacity = 0;
	
	
	if (!(vec1->a = (byte*)malloc(vec2->capacity*vec2->elem_size))) {
		assert(vec1->a != NULL);
		return;
	}

	vec1->size = vec2->size;
	vec1->capacity = vec2->capacity;
	vec1->elem_size = vec2->elem_size;
	vec1->elem_init = vec2->elem_init;
	vec1->elem_free = vec2->elem_free;
	
	if (vec1->elem_init) {
		for (i=0; i<vec1->size; ++i) {
			vec1->elem_init(&vec1->a[i*vec1->elem_size], &vec2->a[i*vec1->elem_size]);
		}
	} else {
		memcpy(vec1->a, vec2->a, vec1->size*vec1->elem_size);
	}
}

int push_void(vector_void* vec, void* a)
{
	byte* tmp;
	size_t tmp_sz;
	if (vec->capacity > vec->size) {
		if (vec->elem_init) {
			vec->elem_init(&vec->a[vec->size*vec->elem_size], a);
		} else {
			memcpy(&vec->a[vec->size*vec->elem_size], a, vec->elem_size);
		}
	} else {
		tmp_sz = VEC_VOID_ALLOCATOR(vec->capacity);
		if (!(tmp = (byte*)realloc(vec->a, vec->elem_size*tmp_sz))) {
			assert(tmp != NULL);
			return 0;
		}
		vec->a = tmp;
		
		if (vec->elem_init) {
			vec->elem_init(&vec->a[vec->size*vec->elem_size], a);
		} else {
			memcpy(&vec->a[vec->size*vec->elem_size], a, vec->elem_size);
		}
		
		vec->capacity = tmp_sz;
	}
	
	vec->size++;
	return 1;
}

void pop_void(vector_void* vec, void* ret)
{
	if (ret) {
		memcpy(ret, &vec->a[(--vec->size)*vec->elem_size], vec->elem_size);
	} else {
		vec->size--;
	}

	if (vec->elem_free) {
		vec->elem_free(&vec->a[vec->size*vec->elem_size]);
	}
}

void* back_void(vector_void* vec)
{
	return &vec->a[(vec->size-1)*vec->elem_size];
}

int extend_void(vector_void* vec, size_t num)
{
	byte* tmp;
	size_t tmp_sz;
	if (vec->capacity < vec->size + num) {
		tmp_sz = vec->capacity + num + VEC_VOID_START_SZ;
		if (!(tmp = (byte*)realloc(vec->a, vec->elem_size*tmp_sz))) {
			assert(tmp != NULL);
			return 0;
		}
		vec->a = tmp;
		vec->capacity = tmp_sz;
	}

	vec->size += num;
	return 1;
}

void* vec_void_get(vector_void* vec, size_t i)
{
	return &vec->a[i*vec->elem_size];
}

int insert_void(vector_void* vec, size_t i, void* a)
{
	byte* tmp;
	size_t tmp_sz;
	if (vec->capacity > vec->size) {
		memmove(&vec->a[(i+1)*vec->elem_size], &vec->a[i*vec->elem_size], (vec->size-i)*vec->elem_size);

		if (vec->elem_init) {
			vec->elem_init(&vec->a[i*vec->elem_size], a);
		} else {
			memcpy(&vec->a[i*vec->elem_size], a, vec->elem_size);
		}
	} else {
		tmp_sz = VEC_VOID_ALLOCATOR(vec->capacity);
		if (!(tmp = (byte*)realloc(vec->a, vec->elem_size*tmp_sz))) {
			assert(tmp != NULL);
			return 0;
		}
		
		vec->a = tmp;
		memmove(&vec->a[(i+1)*vec->elem_size], &vec->a[i*vec->elem_size], (vec->size-i)*vec->elem_size);
		
		if (vec->elem_init) {
			vec->elem_init(&vec->a[i*vec->elem_size], a);
		} else {
			memcpy(&vec->a[i*vec->elem_size], a, vec->elem_size);
		}
		
		vec->capacity = tmp_sz;
	}

	vec->size++;
	return 1;
}

int insert_array_void(vector_void* vec, size_t i, void* a, size_t num)
{
	byte* tmp;
	size_t tmp_sz, j;
	if (vec->capacity < vec->size + num) {
		tmp_sz = vec->capacity + num + VEC_VOID_START_SZ;
		if (!(tmp = (byte*)realloc(vec->a, vec->elem_size*tmp_sz))) {
			assert(tmp != NULL);
			return 0;
		}
		vec->a = tmp;
		vec->capacity = tmp_sz;
	}

	memmove(&vec->a[(i+num)*vec->elem_size], &vec->a[i*vec->elem_size], (vec->size-i)*vec->elem_size);
	if (vec->elem_init) {
		for (j=0; j<num; ++j) {
			vec->elem_init(&vec->a[(j+i)*vec->elem_size], &((byte*)a)[j*vec->elem_size]);
		}
	} else {
		memcpy(&vec->a[i*vec->elem_size], a, num*vec->elem_size);
	}
	vec->size += num;
	return 1;
}

void erase_void(vector_void* vec, size_t start, size_t end)
{
	size_t i;
	size_t d = end - start + 1;
	if (vec->elem_free) {
		for (i=start; i<=end; i++) {
			vec->elem_free(&vec->a[i*vec->elem_size]);
		}
	}
	memmove(&vec->a[start*vec->elem_size], &vec->a[(end+1)*vec->elem_size], (vec->size-1-end)*vec->elem_size);
	vec->size -= d;
}

int reserve_void(vector_void* vec, size_t size)
{
	byte* tmp;
	if (vec->capacity < size) {
		if (!(tmp = (byte*)realloc(vec->a, vec->elem_size*(size+VEC_VOID_START_SZ)))) {
			assert(tmp != NULL);
			return 0;
		}
		vec->a = tmp;
		vec->capacity = size + VEC_VOID_START_SZ;
	}
	return 1;
}

int set_capacity_void(vector_void* vec, size_t size)
{
	size_t i;
	byte* tmp;
	if (size < vec->size) {
		if (vec->elem_free) {
			for (i=vec->size-1; i>=size; i--) {
				vec->elem_free(&vec->a[i*vec->elem_size]);
			}
		}
		vec->size = size;
	}

	vec->capacity = size;

	if (!(tmp = (byte*)realloc(vec->a, vec->elem_size*size))) {
		assert(tmp != NULL);
		return 0;
	}
	vec-> a = tmp;
	return 1;
}

void set_val_sz_void(vector_void* vec, void* val)
{
	size_t i;

	if (vec->elem_free) {
		for(i=0; i<vec->size; i++) {
			vec->elem_free(&vec->a[i*vec->elem_size]);
		}
	}
	
	if (vec->elem_init) {
		for (i=0; i<vec->size; i++) {
			vec->elem_init(&vec->a[i*vec->elem_size], val);
		}
	} else {
		for (i=0; i<vec->size; i++) {
			memcpy(&vec->a[i*vec->elem_size], val, vec->elem_size);
		}
	}
}

void set_val_cap_void(vector_void* vec, void* val)
{
	size_t i;
	if (vec->elem_free) {
		for (i=0; i<vec->size; i++) {
			vec->elem_free(&vec->a[i*vec->elem_size]);
		}
		vec->size = vec->capacity;
	}

	if (vec->elem_init) {
		for (i=0; i<vec->capacity; i++) {
			vec->elem_init(&vec->a[i*vec->elem_size], val);
		}
	} else {
		for (i=0; i<vec->capacity; i++) {
			memcpy(&vec->a[i*vec->elem_size], val, vec->elem_size);
		}
	}
}

void clear_void(vector_void* vec)
{
	size_t i;
	if (vec->elem_free) {
		for (i=0; i<vec->size; ++i) {
			vec->elem_free(&vec->a[i*vec->elem_size]);
		}
	}
	vec->size = 0;
}

void free_vec_void_heap(void* vec)
{
	size_t i;
	vector_void* tmp = (vector_void*)vec;
	if (!tmp)
		return;

	if (tmp->elem_free) {
		for (i=0; i<tmp->size; i++) {
			tmp->elem_free(&tmp->a[i*tmp->elem_size]);
		}
	}
	free(tmp->a);
	free(tmp);
}

void free_vec_void(void* vec)
{
	size_t i;
	vector_void* tmp = (vector_void*)vec;
	if (tmp->elem_free) {
		for (i=0; i<tmp->size; i++) {
			tmp->elem_free(&tmp->a[i*tmp->elem_size]);
		}
	}

	free(tmp->a);

	tmp->size = 0;
	tmp->capacity = 0;
}

#endif
