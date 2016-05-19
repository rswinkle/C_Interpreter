
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


int cvec_i(vector_i* vec, size_t size, size_t capacity);
int cvec_init_i(vector_i* vec, int* vals, size_t num);

vector_i* cvec_i_heap(size_t size, size_t capacity);
vector_i* cvec_init_i_heap(int* vals, size_t num);
void cvec_i_copy(void* dest, void* src);

int cvec_push_i(vector_i* vec, int a);
int cvec_pop_i(vector_i* vec);

int cvec_extend_i(vector_i* vec, size_t num);
int cvec_insert_i(vector_i* vec, size_t i, int a);
int cvec_insert_array_i(vector_i* vec, size_t i, int* a, size_t num);
int cvec_replace_i(vector_i* vec, size_t i, int a);
void cvec_erase_i(vector_i* vec, size_t start, size_t end);
int cvec_reserve_i(vector_i* vec, size_t size);
int cvec_set_cap_i(vector_i* vec, size_t size);
void cvec_set_val_sz_i(vector_i* vec, int val);
void cvec_set_val_cap_i(vector_i* vec, int val);

int* cvec_back_i(vector_i* vec);

void cvec_clear_i(vector_i* vec);
void cvec_free_i_heap(void* vec);
void cvec_free_i(void* vec);




/** Data structure for double vector. */
typedef struct vector_d
{
	double* a;         /**< Array. */
	size_t size;       /**< Current size (amount you use when manipulating array directly). */
	size_t capacity;   /**< Allocated size of array; always >= size. */
} vector_d;

extern size_t VEC_D_START_SZ;


int cvec_d(vector_d* vec, size_t size, size_t capacity);
int cvec_init_d(vector_d* vec, double* vals, size_t num);

vector_d* cvec_d_heap(size_t size, size_t capacity);
vector_d* cvec_init_d_heap(double* vals, size_t num);

void cvec_d_copy(void* dest, void* src);


int cvec_push_d(vector_d* vec, double a);
double cvec_pop_d(vector_d* vec);

int cvec_extend_d(vector_d* vec, size_t num);
int cvec_insert_d(vector_d* vec, size_t i, double a);
int cvec_insert_array_d(vector_d* vec, size_t i, double* a, size_t num);
double cvec_replace_d(vector_d* vec, size_t i, double a);
void cvec_erase_d(vector_d* vec, size_t start, size_t end);
int cvec_reserve_d(vector_d* vec, size_t size);
int cvec_set_cap_d(vector_d* vec, size_t size);
void cvec_set_val_sz_d(vector_d* vec, double val);
void cvec_set_val_cap_d(vector_d* vec, double val);

double* cvec_back_d(vector_d* vec);

void cvec_clear_d(vector_d* vec);
void cvec_free_d_heap(void* vec);
void cvec_free_d(void* vec);



/** Data structure for string vector. */
typedef struct vector_str
{
	char** a;          /**< Array. */
	size_t size;       /**< Current size (amount you use when manipulating array directly). */
	size_t capacity;   /**< Allocated size of array; always >= size. */
} vector_str;


extern size_t VEC_STR_START_SZ;

//char* mystrdup(const char* str);

int cvec_str(vector_str* vec, size_t size, size_t capacity);
int cvec_init_str(vector_str* vec, char** vals, size_t num);

vector_str* cvec_str_heap(size_t size, size_t capacity);
vector_str* cvec_init_str_heap(char** vals, size_t num);

void cvec_str_copy(void* dest, void* src);

int cvec_push_str(vector_str* vec, char* a);
void cvec_pop_str(vector_str* vec, char* ret);

int cvec_extend_str(vector_str* vec, size_t num);
int cvec_insert_str(vector_str* vec, size_t i, char* a);
int cvec_insert_array_str(vector_str* vec, size_t i, char** a, size_t num);
void cvec_replace_str(vector_str* vec, size_t i, char* a, char* ret);
void cvec_erase_str(vector_str* vec, size_t start, size_t end);
int cvec_reserve_str(vector_str* vec, size_t size);
int cvec_set_cap_str(vector_str* vec, size_t size);
void cvec_set_val_sz_str(vector_str* vec, char* val);
void cvec_set_val_cap_str(vector_str* vec, char* val);

char** cvec_back_str(vector_str* vec);

void cvec_clear_str(vector_str* vec);
void cvec_free_str_heap(void* vec);
void cvec_free_str(void* vec);

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

int cvec_void(vector_void* vec, size_t size, size_t capacity, size_t elem_sz, void(*elem_free)(void*), void(*elem_init)(void*, void*));
int cvec_init_void(vector_void* vec, void* vals, size_t num, size_t elem_sz, void(*elem_free)(void*), void(*elem_init)(void*, void*));

vector_void* cvec_void_heap(size_t size, size_t capacity, size_t elem_sz, void (*elem_free)(void*), void(*elem_init)(void*, void*));
vector_void* cvec_init_void_heap(void* vals, size_t num, size_t elem_sz, void (*elem_free)(void*), void(*elem_init)(void*, void*));

void cvec_void_copy(void* dest, void* src);

int cvec_push_void(vector_void* vec, void* val);
void cvec_pop_void(vector_void* vec, void* ret);
void* cvec_get_void(vector_void* vec, size_t i);

int cvec_extend_void(vector_void* vec, size_t num);
int cvec_insert_void(vector_void* vec, size_t i, void* a);
int cvec_insert_array_void(vector_void* vec, size_t i, void* a, size_t num);
void cvec_replace_void(vector_void* vec, size_t i, void* a, void* ret);
void cvec_erase_void(vector_void* vec, size_t start, size_t end);
int cvec_reserve_void(vector_void* vec, size_t size);
int cvec_set_cap_void(vector_void* vec, size_t size);
void cvec_set_val_sz_void(vector_void* vec, void* val);
void cvec_set_val_cap_void(vector_void* vec, void* val);

void* cvec_back_void(vector_void* vec);

void cvec_clear_void(vector_void* vec);
void cvec_free_void_heap(void* vec);
void cvec_free_void(void* vec);


#ifdef __cplusplus
}
#endif

/* header ends */
#endif


#ifdef CVECTOR_IMPLEMENTATION

size_t VEC_I_START_SZ = 50;

#define VEC_I_ALLOCATOR(x) ((x) * 2)




/**
 * Creates a new vector_i on the heap.
 * Vector size set to (size > 0) ? size : 0;
 * Capacity to (capacity > vec->size || (vec->size && capacity == vec->size)) ? capacity : vec->size + VEC_I_START_SZ
 * in other words capacity has to be at least 1 and >= to vec->size of course.
 */
vector_i* cvec_i_heap(size_t size, size_t capacity)
{
	vector_i* vec;
	if (!(vec = (vector_i*)malloc(sizeof(vector_i)))) {
		assert(vec != NULL);
		return NULL;
	}

	vec->size = size;
	vec->capacity = (capacity > vec->size || (vec->size && capacity == vec->size)) ? capacity : vec->size + VEC_I_START_SZ;

	if (!(vec->a = (int*)malloc(vec->capacity*sizeof(int)))) {
		assert(vec->a != NULL);
		free(vec);
		return NULL;
	}

	return vec;
}

/** Create (on the heap) and initialize vector_i with num elements of vals.
 *  Capacity is set to num + VEC_I_START_SZ.
 */
vector_i* cvec_init_i_heap(int* vals, size_t num)
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

/** Same as cvec_i_heap() except the vector passed in was declared on the stack so
 *  it isn't allocated in this function.  Use the cvec_free_i in this case.
 *  This and cvec_init_i should be preferred over the heap versions.
 */
int cvec_i(vector_i* vec, size_t size, size_t capacity)
{
	vec->size = size;
	vec->capacity = (capacity > vec->size || (vec->size && capacity == vec->size)) ? capacity : vec->size + VEC_I_START_SZ;

	if (!(vec->a = (int*)malloc(vec->capacity*sizeof(int)))) {
		assert(vec->a != NULL);
		vec->size = vec->capacity = 0;
		return 0;
	}

	return 1;
}


/** Same as cvec_init_i_heap() except the vector passed in was declared on the stack so
 *  it isn't allocated in this function.
 */
int cvec_init_i(vector_i* vec, int* vals, size_t num)
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


/** Makes dest an identical copy of src.  The parameters
 *  are void so it can be used as the constructor when making
 *  a vector of vector_i's.  Assumes dest (the structure)
 *  is already allocated (probably on the stack) and that
 *  capacity is 0 (ie the array doesn't need to be freed).
 */
void cvec_i_copy(void* dest, void* src)
{
	vector_i* vec1 = (vector_i*)dest;
	vector_i* vec2 = (vector_i*)src;
	
	vec1->size = 0;
	vec1->capacity = 0;
	
	/*not much else we can do here*/
	if (!(vec1->a = (int*)malloc(vec2->capacity*sizeof(int)))) {
		assert(vec1->a != NULL);
		return;
	}
	
	memcpy(vec1->a, vec2->a, vec2->size*sizeof(int));
	vec1->size = vec2->size;
	vec1->capacity = vec2->capacity;
}





/**
 * Append a to end of vector (size increased 1).
 * Capacity is increased by doubling when necessary.
 */
int cvec_push_i(vector_i* vec, int a)
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



/** Remove and return the last element (size decreased 1).*/
int cvec_pop_i(vector_i* vec)
{
	return vec->a[--vec->size];
}

/** Return pointer to last element */
int* cvec_back_i(vector_i* vec)
{
	return &vec->a[vec->size-1];
}



/** Increase the size of the array num items.  Items
 *  are not initialized to anything */
int cvec_extend_i(vector_i* vec, size_t num)
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




/**
 * Insert a at index i (0 based).
 * Everything from that index and right is shifted one to the right.
 */
int cvec_insert_i(vector_i* vec, size_t i, int a)
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


/**
 * Insert the first num elements of array a at index i.
 * Note that it is the user's responsibility to pass in valid
 * arguments.  Also memcpy is used so don't try to insert
 * part of the vector array into itself (that would require memmove)
 */
int cvec_insert_array_i(vector_i* vec, size_t i, int* a, size_t num)
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


/** Replace value at index i with a, return original value. */
int cvec_replace_i(vector_i* vec, size_t i, int a)
{
	int tmp = vec->a[i];
	vec->a[i] = a;
	return tmp;
}


/**
 * Erases elements from start to end inclusive.
 * Example cvec_erase_i(myvec, 1, 3) would remove elements at 1, 2, and 3 and the element
 * that was at index 4 would now be at 1 etc.
 */
void cvec_erase_i(vector_i* vec, size_t start, size_t end)
{
	size_t d = end - start + 1;
	memmove(&vec->a[start], &vec->a[end+1], (vec->size-1-end)*sizeof(int));
	vec->size -= d;
}


/** Make sure capacity is at least size(parameter not member). */
int cvec_reserve_i(vector_i* vec, size_t size)
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



/** Set capacity to size.
 * You will lose data if you shrink the capacity below the current size.
 * If you do, the size will be set to capacity of course.
*/
int cvec_set_cap_i(vector_i* vec, size_t size)
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



/** Set all size elements to val. */
void cvec_set_val_sz_i(vector_i* vec, int val)
{
	size_t i;
	for (i=0; i<vec->size; i++) {
		vec->a[i] = val;
	}
}


/** Fills entire allocated array (capacity) with val. */
void cvec_set_val_cap_i(vector_i* vec, int val)
{
	size_t i;
	for (i=0; i<vec->capacity; i++) {
		vec->a[i] = val;
	}
}


/** Sets size to 0 (does not clear contents).*/
void cvec_clear_i(vector_i* vec) { vec->size = 0; }

/** Frees everything so don't use vec after calling this. */
void cvec_free_i_heap(void* vec)
{
	vector_i* tmp = (vector_i*)vec;
	free(tmp->a);
	free(tmp);
}

/** Frees the internal array and sets size and capacity to 0 */
void cvec_free_i(void* vec)
{
	vector_i* tmp = (vector_i*)vec;
	free(tmp->a);
	tmp->size = 0;
	tmp->capacity = 0;
}

size_t VEC_D_START_SZ = 50;


#define VEC_D_ALLOCATOR(x) ((x) * 2)




/**
 * Creates a new vector_d on the heap.
 * Vector size set to (size > 0) ? size : 0;
 * Capacity to (capacity > vec->size || (vec->size && capacity == vec->size)) ? capacity : vec->size + VEC_D_START_SZ
 * in other words capacity has to be at least 1 and >= to vec->size of course.
 */
vector_d* cvec_d_heap(size_t size, size_t capacity)
{
	vector_d* vec;
	
	if (!(vec = (vector_d*)malloc(sizeof(vector_d)))) {
		assert(vec != NULL);
		return NULL;
	}

	vec->size = size;
	vec->capacity = (capacity > vec->size || (vec->size && capacity == vec->size)) ? capacity : vec->size + VEC_D_START_SZ;

	if (!(vec->a = (double*)malloc(vec->capacity*sizeof(double)))) {
		assert(vec->a != NULL);
		free(vec);
		return NULL;
	}

	return vec;
}


/** Create (on the heap) and initialize vector_d with num elements of vals.
 *  Capacity is set to num + VEC_D_START_SZ.
 */
vector_d* cvec_init_d_heap(double* vals, size_t num)
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


/** Same as cvec_d_heap() except the vector passed in was declared on the stack so
 *  it isn't allocated in this function.  Use the cvec_free_d in this case.
 *  This and cvec_init_d should be preferred over the heap versions.
 */
int cvec_d(vector_d* vec, size_t size, size_t capacity)
{
	vec->size = size;
	vec->capacity = (capacity > vec->size || (vec->size && capacity == vec->size)) ? capacity : vec->size + VEC_D_START_SZ;

	if (!(vec->a = (double*)malloc(vec->capacity*sizeof(double)))) {
		assert(vec->a != NULL);
		vec->size = vec->capacity = 0;
		return 0;
	}

	return 1;
}

/** Same as cvec_init_d_heap() except the vector passed in was declared on the stack so
 *  it isn't allocated in this function.  Use the cvec_free_d in this case.
 */
int cvec_init_d(vector_d* vec, double* vals, size_t num)
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

/** Makes dest an identical copy of src.  The parameters
 *  are void so it can be used as the constructor when making
 *  a vector of vector_d's.  Assumes dest (the structure)
 *  is already allocated (probably on the stack) and that
 *  capacity is 0 (ie the array doesn't need to be freed).
 */
void cvec_d_copy(void* dest, void* src)
{
	vector_d* vec1 = (vector_d*)dest;
	vector_d* vec2 = (vector_d*)src;
	
	vec1->size = 0;
	vec1->capacity = 0;
	
	/*not much else we can do here*/
	if (!(vec1->a = (double*)malloc(vec2->capacity*sizeof(double)))) {
		assert(vec1->a != NULL);
		return;
	}
	
	memcpy(vec1->a, vec2->a, vec2->size*sizeof(double));
	vec1->size = vec2->size;
	vec1->capacity = vec2->capacity;
}







/** Append a to end of vector (size increased 1).
 * Capacity is increased by doubling when necessary.
 */
int cvec_push_d(vector_d* vec, double a)
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


/** Remove and return the last element (size decreased 1).*/
double cvec_pop_d(vector_d* vec)
{
	return vec->a[--vec->size];
}


/** Return pointer to last element */
double* cvec_back_d(vector_d* vec)
{
	return &vec->a[vec->size-1];
}




/** Increase the size of the array num items.  Items
 *  are not initialized to anything */
int cvec_extend_d(vector_d* vec, size_t num)
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




/**
 * Insert a at index i (0 based).
 * Everything from that index and right is shifted one to the right.
 */
int cvec_insert_d(vector_d* vec, size_t i, double a)
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

/**
 * Insert the first num elements of array a at index i.
 * Note that it is the user's responsibility to pass in valid
 * arguments.  Also memcpy is used so don't try to insert
 * part of the vector array into itself (that would require memmove)
 */
int cvec_insert_array_d(vector_d* vec, size_t i, double* a, size_t num)
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


/** Replace value at index i with a, return original value. */
double cvec_replace_d(vector_d* vec, size_t i, double a)
{
	double tmp = vec->a[i];
	vec->a[i] = a;
	return tmp;
}

/**
 * Erases elements from start to end inclusive.
 * Example cvec_erase_d(myvec, 1, 3) would remove elements at 1, 2, and 3 and the element
 * that was at index 4 would now be at 1 etc.
 */
void cvec_erase_d(vector_d* vec, size_t start, size_t end)
{
	size_t d = end - start + 1;
	memmove(&vec->a[start], &vec->a[end+1], (vec->size-1-end)*sizeof(double));
	vec->size -= d;
}


/** Make sure capacity is at least size(parameter not member). */
int cvec_reserve_d(vector_d* vec, size_t size)
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


/** Set capacity to size.
 * You will lose data if you shrink the capacity below the current size.
 * If you do, the size will be set to capacity of course.
*/
int cvec_set_cap_d(vector_d* vec, size_t size)
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


/** Set all size elements to val. */
void cvec_set_val_sz_d(vector_d* vec, double val)
{
	size_t i;
	for(i=0; i<vec->size; i++) {
		vec->a[i] = val;
	}
}


/** Fills entire allocated array (capacity) with val. */
void cvec_set_val_cap_d(vector_d* vec, double val)
{
	size_t i;
	for(i=0; i<vec->capacity; i++) {
		vec->a[i] = val;
	}
}



/** Sets size to 0 (does not clear contents).*/
void cvec_clear_d(vector_d* vec) { vec->size = 0; }


/** Frees everything so don't use vec after calling this. */
void cvec_free_d_heap(void* vec)
{
	vector_d* tmp = (vector_d*)vec;
	free(tmp->a);
	free(tmp);
}

/** Frees the internal array and sets size and capacity to 0 */
void cvec_free_d(void* vec)
{
	vector_d* tmp = (vector_d*)vec;
	free(tmp->a);
	tmp->size = 0;
	tmp->capacity = 0;
}
size_t VEC_STR_START_SZ = 20;

#define VEC_STR_ALLOCATOR(x) ((x) * 2)

/** Useful utility function since strdup isn't in standard C.
char* mystrdup(const char* str)
{
	size_t len = strlen(str);
	char* temp = (char*)calloc(len+1, sizeof(char));
	if (!temp) {
		assert(temp != NULL);
		return NULL;
	}
	
	return (char*)memcpy(temp, str, len);  /* memcpy returns to, and (char**)calloc already nulled last char
}
*/


/**
 * Create a new vector_str on the heap.
 * Vector size set to (size > 0) ? size : 0;
 * Capacity to (capacity > vec->size || (vec->size && capacity == vec->size)) ? capacity : vec->size + VEC_STR_START_SZ
 * in other words capacity has to be at least 1 and >= to vec->size of course.
 * Note: vector_str does not copy pointers passed in but duplicates the strings
 * they point to (using mystrdup()) so you don't have to worry about freeing
 * or changing the contents of variables that you've pushed or inserted; it
 * won't affect the values vector.
 */
vector_str* cvec_str_heap(size_t size, size_t capacity)
{
	vector_str* vec;
	if (!(vec = (vector_str*)malloc(sizeof(vector_str)))) {
		assert(vec != NULL);
		return NULL;
	}

	vec->size = size;
	vec->capacity = (capacity > vec->size || (vec->size && capacity == vec->size)) ? capacity : vec->size + VEC_STR_START_SZ;

	/* calloc here because if we free before poppirg/erasing and since nothing is
	 * allocated these need to be NULL to not cause problems */
	if (!(vec->a = (char**)calloc(vec->capacity, sizeof(char*)))) {
		assert(vec->a != NULL);
		free(vec);
		return NULL;
	}

	return vec;
}


/** Create (on the heap) and initialize vector_str with num elements of vals.
 */
vector_str* cvec_init_str_heap(char** vals, size_t num)
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


/** Same as cvec_str_heap() except the vector passed in was declared on the stack so
 *  it isn't allocated in this function.  Use the cvec_free_str in this case
 *  This and cvec_init_str should be preferred over the heap versions.
 */
int cvec_str(vector_str* vec, size_t size, size_t capacity)
{
	vec->size = size;
	vec->capacity = (capacity > vec->size || (vec->size && capacity == vec->size)) ? capacity : vec->size + VEC_STR_START_SZ;

	/* (char**)calloc here because if we free before popping/erasing and since nothing is
	 * allocated these need to be NULL to not cause problems */
	if (!(vec->a = (char**)calloc(vec->capacity, sizeof(char*)))) {
		assert(vec->a != NULL);
		vec->size = vec->capacity = 0;
		return 0;
	}

	return 1;
}

/** Same as cvec_init_str() except the vector passed in was declared on the stack so
 *  it isn't allocated in this function.  Use the cvec_free_str in this case
 */
int cvec_init_str(vector_str* vec, char** vals, size_t num)
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


/** Makes dest an identical copy of src.  The parameters
 *  are void so it can be used as the constructor when making
 *  a vector of vector_str's.  Assumes dest (the structure)
 *  is already allocated (probably on the stack) and that
 *  capacity is 0 (ie the array doesn't need to be freed).
 */
void cvec_str_copy(void* dest, void* src)
{
	size_t i;
	vector_str* vec1 = (vector_str*)dest;
	vector_str* vec2 = (vector_str*)src;
	
	vec1->size = 0;
	vec1->capacity = 0;
	
	/*not much else we can do here*/
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



/**
 * Append a to end of vector (size increased 1).
 * Capacity is increased by doubling when necessary.
 */
int cvec_push_str(vector_str* vec, char* a)
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

/** Remove the last element (size decreased 1).
 *  String is freed.  If ret != NULL strcpy the last element into ret.
 *  It is the user's responsibility to make sure ret can receive it without error
 *  (ie ret has adequate space.) */
void cvec_pop_str(vector_str* vec, char* ret)
{
	if (ret)
		strcpy(ret, vec->a[--vec->size]);
	free(vec->a[vec->size]);
}

/** Return pointer to last element */
char** cvec_back_str(vector_str* vec)
{
	return &vec->a[vec->size-1];
}






/** Increase the size of the array num items.  Items
 *  are memset to NULL since they will be freed when
    popped or the vector is freed.*/
int cvec_extend_str(vector_str* vec, size_t num)
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




/**
 * Insert a at index i (0 based).
 * Everything from that index and right is shifted one to the right.
 */
int cvec_insert_str(vector_str* vec, size_t i, char* a)
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


/**
 * Insert the first num elements of array a at index i.
 * Note that it is the user's responsibility to pass in valid
 * arguments.
 */
int cvec_insert_array_str(vector_str* vec, size_t i, char** a, size_t num)
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

/**
 * Replace string at i with a. If ret != NULL, strcpy the old str to it.
 * See cvec_pop_str warning
 * */
void cvec_replace_str(vector_str* vec, size_t i, char* a, char* ret)
{
	if (ret)
		strcpy(ret, vec->a[i]);
	free(vec->a[i]);
	vec->a[i] = mystrdup(a);
}


/**
 * Erases strings from start to end inclusive.
 * Example erases(myvec, 1, 3) would free and remove strings at 1, 2, and 3 and the string
 * that was at index 4 would now be at 1 etc.
 */
void cvec_erase_str(vector_str* vec, size_t start, size_t end)
{
	size_t i;
	size_t d = end - start + 1;
	for (i=start; i<=end; i++) {
		free(vec->a[i]);
	}
	
	memmove(&vec->a[start], &vec->a[end+1], (vec->size-1-end)*sizeof(char*));
	vec->size -= d;
}





/** Makes sure the vector capacity is >= size (parameter not member). */
int cvec_reserve_str(vector_str* vec, size_t size)
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

/** Set capacity to size.
 * You will lose data if you shrink the capacity below the current size.
 * If you do, the size will be set to capacity of course.
*/
int cvec_set_cap_str(vector_str* vec, size_t size)
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



/** Sets all size elements to val. */
void cvec_set_val_sz_str(vector_str* vec, char* val)
{
	size_t i;
	for(i=0; i<vec->size; i++) {
		free(vec->a[i]);

		/* not worth checking/(char**)reallocing to me */
		vec->a[i] = mystrdup(val);
	}
}


/** Fills entire allocated array (capacity) with val.  Size is set
 * to capacity in this case because strings are individually dynamically allocated.
 * This is different from vector_i, vector_d and vector (without a free function) where the size stays the same.
   TODO  Remove this function?  even more unnecessary than for vector_i and vector_d and different behavior*/
void cvec_set_val_cap_str(vector_str* vec, char* val)
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


/** Clears the contents of vector (frees all strings) and sets size to 0. */
void cvec_clear_str(vector_str* vec)
{
	int i;
	for (i=0; i<vec->size; i++) {
		free(vec->a[i]);
	}
	
	vec->size = 0;
}


/** Frees contents (individual strings and array) and frees vector so don't use after calling this. */
void cvec_free_str_heap(void* vec)
{
	size_t i;
	vector_str* tmp = (vector_str*)vec;
	for (i=0; i<tmp->size; i++) {
		free(tmp->a[i]);
	}
	
	free(tmp->a);
	free(tmp);
}


/** Frees the internal array and sets size and capacity to 0 */
void cvec_free_str(void* vec)
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




/*  general vector */

/**
 * Creates a new vector on the heap.
 * Vector size set to (size > 0) ? size : 0;
 * Capacity to (capacity > vec->size || (vec->size && capacity == vec->size)) ? capacity : vec->size + VEC_VOID_START_SZ
 * in other words capacity has to be at least 1 and >= to vec->size of course.
 * elem_sz is the size of the type you want to store ( ie sizeof(T) where T is your type ).
 * You can pass in a function, elem_free, to be called on every element before it is removed
 * from the vector to free any dynamically allocated memory.  For example if you passed
 * in sizeof(char*) for elem_sz, and wrappers around the standard free(void*) function for elem_free
 * and strdup (or mystrdup in this project) for elem_init you could
 * make vector work exactly like vector_s.  Pass in NULL, to not use the function parameters.
 *
 * All functions call elem_free before overwriting/popping/erasing elements if elem_free is provided.
 *
 * elem_init is only used in set_val_sz and set_val_cap because in those cases you are setting many elements
 * to a single "value" and using the elem_init functionality you can provide what amounts to a copy constructor
 * which duplicates dynamically allocated memory instead of just copying the pointer ie just like strdup
 * or mystrdup does with a string.  This allows the free function to work correctly when called on all those
 * elements.  If you didn't provide an elem_init function but did provide a free function, then
 * after calling one of the set_val functions, eventually the free function would be called on all those
 * elements and you'd get a double free or corruption error.
 *
 * See the other functions and the tests for more behavioral/usage details.
 */
vector_void* cvec_void_heap(size_t size, size_t capacity, size_t elem_sz, void(*elem_free)(void*), void(*elem_init)(void*, void*))
{
	vector_void* vec;
	if (!(vec = (vector_void*)malloc(sizeof(vector_void)))) {
		assert(vec != NULL);
		return NULL;
	}

	vec->size = size;
	vec->capacity = (capacity > vec->size || (vec->size && capacity == vec->size)) ? capacity : vec->size + VEC_VOID_START_SZ;

	vec->elem_size = elem_sz;
	
	/*not calloc here and init_vec as in vector_s because elem_free cannot be calling free directly*/
	if (!(vec->a = (byte*)malloc(vec->capacity*elem_sz))) {
		assert(vec->a != NULL);
		free(vec);
		return NULL;
	}

	vec->elem_free = elem_free;
	vec->elem_init = elem_init;

	return vec;
}



/** Create (on the heap) and initialize vector with num elements of vals.
 *  elem_sz is the size of the type you want to store ( ie sizeof(T) where T is your type ).
 *  See cvec_void_heap() for more information about the elem_free and elem_init parameters.
 */
vector_void* cvec_init_void_heap(void* vals, size_t num, size_t elem_sz, void(*elem_free)(void*), void(*elem_init)(void*, void*))
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

/** Same as cvec_void_heap() except the vector passed in was declared on the stack so
 *  it isn't allocated in this function.  Use the cvec_free_void in that case
 */
int cvec_void(vector_void* vec, size_t size, size_t capacity, size_t elem_sz, void(*elem_free)(void*), void(*elem_init)(void*, void*))
{
	vec->size = size;
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

/** Same as init_vec_heap() except the vector passed in was declared on the stack so
 *  it isn't allocated in this function.  Use the cvec_free_void in this case
 */
int cvec_init_void(vector_void* vec, void* vals, size_t num, size_t elem_sz, void(*elem_free)(void*), void(*elem_init)(void*, void*))
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


/** Makes dest an identical copy of src.  The parameters
 *  are void so it can be used as the constructor when making
 *  a vector of generic vector's. (I would recommend against doing that, and using
 *  generate_code.py to make your own vector type and do a vector of those
 *  instead).  Assumes dest (the structure)
 *  is already allocated (probably on the stack) and that
 *  capacity is 0 (ie the array doesn't need to be freed).
 */
void cvec_void_copy(void* dest, void* src)
{
	size_t i;
	vector_void* vec1 = (vector_void*)dest;
	vector_void* vec2 = (vector_void*)src;
	
	vec1->size = 0;
	vec1->capacity = 0;
	
	/*not much else we can do here*/
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


/** Append a to end of vector (size increased 1).
 * Capacity is increased by doubling when necessary.
 */
int cvec_push_void(vector_void* vec, void* a)
{
	byte* tmp;
	size_t tmp_sz;
	if (vec->capacity == vec->size) {
		tmp_sz = VEC_VOID_ALLOCATOR(vec->capacity);
		if (!(tmp = (byte*)realloc(vec->a, vec->elem_size*tmp_sz))) {
			assert(tmp != NULL);
			return 0;
		}
		vec->a = tmp;
		vec->capacity = tmp_sz;
	}
	if (vec->elem_init) {
		vec->elem_init(&vec->a[vec->size*vec->elem_size], a);
	} else {
		memmove(&vec->a[vec->size*vec->elem_size], a, vec->elem_size);
	}
	
	vec->size++;
	return 1;
}


/** Remove the last element (size decreased 1).
 * Copy the element into ret.  This function assumes
 * that ret is not NULL and is large accept the element and just memmove's it in.
 * Similar to pop_backs it is users responsibility.
 */
void cvec_pop_void(vector_void* vec, void* ret)
{
	vec->size--;
	if (ret) {
		memmove(ret, &vec->a[vec->size*vec->elem_size], vec->elem_size);
	}
	if (vec->elem_free) {
		vec->elem_free(&vec->a[vec->size*vec->elem_size]);
	}
}

/** Return pointer to last element */
void* cvec_back_void(vector_void* vec)
{
	return &vec->a[(vec->size-1)*vec->elem_size];
}




/** Increase the size of the array num items.  Items
 *  are not initialized to anything! */
int cvec_extend_void(vector_void* vec, size_t num)
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


/** Return a void pointer to the ith element.
  * Another way to get elements from vector that is used in vector_tests.c
  * is a macro like this one
  * #define GET_ELEMENT(X,Y,TYPE) ((TYPE*)&X.a[Y*X.elem_size])
*/
void* cvec_get_void(vector_void* vec, size_t i)
{
	return &vec->a[i*vec->elem_size];
}



/**
 * Insert a at index i (0 based).
 * Everything from that index and right is shifted one to the right.
 */
int cvec_insert_void(vector_void* vec, size_t i, void* a)
{
	byte* tmp;
	size_t tmp_sz;
	if (vec->capacity == vec->size) {
		tmp_sz = VEC_VOID_ALLOCATOR(vec->capacity);
		if (!(tmp = (byte*)realloc(vec->a, vec->elem_size*tmp_sz))) {
			assert(tmp != NULL);
			return 0;
		}
		
		vec->a = tmp;
		vec->capacity = tmp_sz;
	}
	memmove(&vec->a[(i+1)*vec->elem_size], &vec->a[i*vec->elem_size], (vec->size-i)*vec->elem_size);

	if (vec->elem_init) {
		vec->elem_init(&vec->a[i*vec->elem_size], a);
	} else {
		memmove(&vec->a[i*vec->elem_size], a, vec->elem_size);
	}

	vec->size++;
	return 1;
}

/**
 * Insert the first num elements of array a at index i.
 * Note that it is the user's responsibility to pass in val_id
 * arguments.  Also memcpy is used (when there is no elem_init function)
 * so don't try to insert part of the vector array into itself
 * (that would require memmove)
 */
int cvec_insert_array_void(vector_void* vec, size_t i, void* a, size_t num)
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

/**
 * Replace value at i with a, return old value in ret if non-NULL.
 */
void cvec_replace_void(vector_void* vec, size_t i, void* a, void* ret)
{
	if (ret)
		memmove(ret, &vec->a[i*vec->elem_size], vec->elem_size);
	memmove(&vec->a[i*vec->elem_size], a, vec->elem_size);
}

/**
 * Erases elements from start to end inclusive.
 * Example cvec_erase_void(myvec, 1, 3) would free (if an elem_free function was provided) and remove elements at 1, 2, and 3 and the element
 * that was at index 4 would now be at 1 etc.
 */
void cvec_erase_void(vector_void* vec, size_t start, size_t end)
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


/** Makes sure capacity >= size (the parameter not the member). */
int cvec_reserve_void(vector_void* vec, size_t size)
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


/** Set capacity to size.
 * You will lose data if you shrink the capacity below the current size.
 * If you do, the size will be set to capacity of course.
*/
int cvec_set_cap_void(vector_void* vec, size_t size)
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



/** Set all size elements to val. */
void cvec_set_val_sz_void(vector_void* vec, void* val)
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
			memmove(&vec->a[i*vec->elem_size], val, vec->elem_size);
		}
	}
}


/** Fills entire allocated array (capacity) with val.  If you set a free function
 * then size is set to capacity like vector_s for the same reason, ie I need to know
 * that the free function needs to be called on those elements.
 * TODO Remove this function?  Same reason as set_val_caps.
 */
void cvec_set_val_cap_void(vector_void* vec, void* val)
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
			memmove(&vec->a[i*vec->elem_size], val, vec->elem_size);
		}
	}
}


/** Sets size to 0 (does not change contents unless elem_free is set
 *  then it will free all size elements as in vector_s). */
void cvec_clear_void(vector_void* vec)
{
	size_t i;
	if (vec->elem_free) {
		for (i=0; i<vec->size; ++i) {
			vec->elem_free(&vec->a[i*vec->elem_size]);
		}
	}
	vec->size = 0;
}


/** Frees everything so don't use vec after calling this. If you set a free function
 * it will be called on all size elements of course. */
void cvec_free_void_heap(void* vec)
{
	size_t i;
	vector_void* tmp = (vector_void*)vec;
	if (!tmp) return;

	if (tmp->elem_free) {
		for (i=0; i<tmp->size; i++) {
			tmp->elem_free(&tmp->a[i*tmp->elem_size]);
		}
	}
	free(tmp->a);
	free(tmp);
}


/** Frees the internal array and sets size and capacity to 0 */
void cvec_free_void(void* vec)
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



/*! \mainpage CVector notes
 *

\section Intro
This is a relatively simple ANSI compliant C vector library with specific structures and
functions for int's, double's and string's and support for all other types
using a generic structure where the type is passed in as void* and stored in a byte array
(to avoid dereferencing void* warnings and frequent casting) .
The generic vector is very flexible and allows you to provide free and init functions
if you like that it will call at appropriate times similar to the way C++ containers
will call destructors.

Other modifiable parameters are at the top of vector_*.c
<pre>
size_t VEC_I_START_SZ = 50;
size_t VEC_D_START_SZ = 50;
size_t VEC_VOID_START_SZ = 20;
size_t VEC_STR_START_SZ = 20;

#define VEC_I_ALLOCATOR(x) ((x) * 2)
#define VEC_D_ALLOCATOR(x) ((x) * 2)
#define VEC_STR_ALLOCATOR(x) ((x) * 2)
#define VEC_VOID_ALLOCATOR(x) ((x) * 2)
</pre>
The allocator macros are used in all functions that increase the size by 1.
In others (constructors, insert_array, reserve) VEC_X_START_SZ is the amount
extra allocated.


There are also 2 templates, one for basic types and one for types that contain
dynamically allocated memory and you might want a free and/or init function.
In other words the first template is based off vector_i and the second is based
off of vector_void, so look at the corresponding documentation for behavior.
There are actually 2 varieties of each template, one all-in-one header variety that works
like cvector.h, and the other generates a matching c/h pair.

They are located in vector_template.h, vector_template2.h, vector_template3.c/h and
vector_template4.c/h.

To generate your own vector files for a type just run:
<pre>
python3 generate_code.py yourtype
</pre>

which will generate the results for all templates so just delete the ones
you don't want.

vector_short and vector_f_struct are examples of the process and
how to add it to the testing.


\section des_notes Design Notes
Memory allocations are checked and asserted.  If not in debug mode (ie NDEBUG is defined)
0 is returned on allocation failure.

No other error checking is performed.  If you pass bad parameters, bad things will probably happen.
This is consistent with my belief that it is the caller's responsibility to pass valid arguments
and library code shouldn't be larger/uglier/slower for everyone just to pretty print errors.  This
is also consistent with the C standard library where, for the most part, passing invalid parameters
results in undefined behavior (see section 4.1.6 in C89, 7.1.4 in C99 and C11).

The back functions simply return the address of size - 1.  This is fine even if your size is zero
for the use of <= back_i(myvec) since the beginning of the array will already be > back.  If I were
to return NULL in the case of size 0, you'd just exchange a possible size check before the call for
a possible NULL check after the call.  I choose this way because it doesn't add an if check
to the function so it's smaller/faster, I think the <= use case is more likely, and it's easier
and more normal to know when your vector is empty than to remember to check for NULL after the fact.

The insert functions (insert_i and insert_array_i for example) do allow you to insert at the end.
The memmove inside the functions will simply move 0 bytes if you pass the current size as the index.
C99 and C11 guarrantee this behavior in the standard (and thus C++ does as well).  Though I wrote
this library to be compliant with C89, which does not guarrantee this behavior, I think
it's safe to assume they'd use the same implementation since it doesn't contradict C89 and it
just makes sense.



\section Building
I use premake so the command on linux is premake4 gmake which
will generate a build directory.  cd into that and run make
or make config=release.  I have not tried it on windows though
it should work (well I'm not sure about CUnit ...).

There is no output of any kind, no errors or warnings.


It has been relatively well tested using Cunit tests which all pass.
I've also run it under valgrind and there are no memory leaks.

<pre>
valgrind --leak-check=full -v ./vector

==17650== 
==17650== HEAP SUMMARY:
==17650==     in use at exit: 0 bytes in 0 blocks
==17650==   total heap usage: 5,146 allocs, 5,146 frees, 936,924 bytes allocated
==17650== 
==17650== All heap blocks were freed -- no leaks are possible
==17650== 
==17650== ERROR SUMMARY: 0 errors from 0 contexts (suppressed: 2 from 2)
</pre>

You can probably get Cunit from your package manager but
if you want to get the most up to date version of CUnit go here:

http://cunit.sourceforge.net/index.html
http://sourceforge.net/projects/cunit/

I'm using version 2.1-3.


\section Usage
To actually use the library just copy the appropriate c/h file pair(s) to your project
or just use cvector.h.
To get a good idea of how to use the library and see it in action and how it should
behave, look at vector_tests.c

\section LICENSE
CVector is licensed under the MIT License.

Copyright (c) 2011-2016 Robert Winkler

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated
documentation files (the "Software"), to deal in the Software without restriction, including without limitation
the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and
to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED
TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF
CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
IN THE SOFTWARE.
 *
 *
 */







#endif
