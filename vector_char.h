#ifndef VECTOR_char_H
#define VECTOR_char_H

#include <stdlib.h>
#include <stdio.h>
#include <string.h>



/** Data structure for char vector. */
typedef struct vector_char
{
	char* a;           /**< Array. */
	size_t size;       /**< Current size (amount you use when manipulating array directly). */
	size_t capacity;   /**< Allocated size of array; always >= size. */
} vector_char;



extern size_t VECTOR_char_SZ;

int vec_char(vector_char* vec, size_t size, size_t capacity);
int init_vec_char(vector_char* vec, char* vals, size_t num);

vector_char* vec_char_heap(size_t size, size_t capacity);
vector_char* init_vec_char_heap(char* vals, size_t num);

void vec_char_copy(void* dest, void* src);

int push_char(vector_char* vec, char a);
char pop_char(vector_char* vec);

int extend_char(vector_char* vec, size_t num);
int insert_char(vector_char* vec, size_t i, char a);
int insert_array_char(vector_char* vec, size_t i, char* a, size_t num);
void erase_char(vector_char* vec, size_t start, size_t end);
int reserve_char(vector_char* vec, size_t size);
int set_capacity_char(vector_char* vec, size_t size);
void set_val_sz_char(vector_char* vec, char val);
void set_val_cap_char(vector_char* vec, char val);

char* back_char(vector_char* vec);

void clear_char(vector_char* vec);
void free_vec_char_heap(void* vec);
void free_vec_char(void* vec);



#endif
