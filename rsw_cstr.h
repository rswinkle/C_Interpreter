#ifndef RSW_CSTR_H
#define RSW_CSTR_H

#include <stdlib.h>
#include <stdio.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct rsw_cstr
{
	char* a;           /**< Array. */
	size_t size;       /**< current size, ie what strlen() would return or std::string::size() */
	size_t capacity;   /**< Allocated size of array; always >= size + 1 (for '\0'). */
} rsw_cstr;


#define SET_CSTR(str, data, sz, cap) \
	(str).a = (data); \
	(str).size = (sz); \
	(str).capacity = (cap)

extern size_t CSTR_ST_SZ;

int init_cstr(rsw_cstr* str);
int init_cstr_cap(rsw_cstr* str, size_t capacity);
int init_cstr_sz(rsw_cstr* str, size_t size, int val);
int init_cstr_sz_cap(rsw_cstr* str, size_t size, int val, size_t capacity);
int init_cstr_str(rsw_cstr* str, const char* start, size_t len);

//rsw_cstr* vec_char_heap(size_t size, size_t capacity);
//rsw_cstr* init_vec_char_heap(char* vals, size_t num);

void cstr_copy(void* dest, void* src);

int cstr_push(rsw_cstr* str, char a);
char cstr_pop(rsw_cstr* str);

int cstr_extend(rsw_cstr* str, size_t num, char a);
int cstr_insert(rsw_cstr* str, size_t i, char a);

int cstr_insert_str(rsw_cstr* str, size_t i, char* a, size_t len);
int cstr_insert_cstr(rsw_cstr* str, size_t i, rsw_cstr* a_str);

int cstr_concatenate(rsw_cstr* str, const char* a, size_t len);
int cstr_concatenate_cstr(rsw_cstr* str, rsw_cstr* a_str);

void cstr_erase(rsw_cstr* str, size_t start, size_t end);

int cstr_reserve(rsw_cstr* str, size_t size);
int cstr_set_capacity(rsw_cstr* str, size_t size); 
void cstr_set_val_sz(rsw_cstr* str, char val);
void cstr_set_val_cap(rsw_cstr* str, char val);


void cstr_clear(rsw_cstr* str);
void free_cstr_heap(void* str);
void free_cstr(void* str);

int cstr_set_str(rsw_cstr* str, const char* a, size_t len);

rsw_cstr* cstr_ltrim(rsw_cstr* str);
rsw_cstr* cstr_rtrim(rsw_cstr* str);
rsw_cstr* cstr_trim(rsw_cstr* str);

rsw_cstr slice_cstr(rsw_cstr* str, long start, long end);
rsw_cstr cstr_substr(rsw_cstr* str, size_t index, size_t len);
int cstr_resize(rsw_cstr* str, size_t size, int val);

size_t cstr_find(rsw_cstr* str, rsw_cstr* needle);
size_t cstr_find_start_at(rsw_cstr* str, rsw_cstr* needle, size_t start);
size_t cstr_find_str(rsw_cstr* str, char* needle);
size_t cstr_find_str_start_at(rsw_cstr* str, char* needle, size_t start);

int cstr_replace(rsw_cstr* str, char* find, char* a, size_t num);
int cstr_replace_substr(rsw_cstr* str, size_t index, size_t num, const char* a, size_t len);
int cstr_replace_substr_cstr(rsw_cstr* str, size_t index, size_t num, rsw_cstr* str2);

int cstr_split(rsw_cstr* str, rsw_cstr* delim, rsw_cstr** results, size_t* num_results);


int file_open_read_cstr(const char* filename, rsw_cstr* out);
int file_read_cstr(FILE* file, rsw_cstr* out);

int file_open_read_new_cstr(const char* filename, rsw_cstr* str);
int file_read_new_cstr(FILE* file, rsw_cstr* str);

int file_open_write_cstr(const char* filename, rsw_cstr* out);
int file_write_cstr(FILE* file, rsw_cstr* out);
int file_open_readlines_cstr(const char* filename, rsw_cstr** lines, size_t* num_results);

int freadstring_into_cstr(FILE* input, int delim, rsw_cstr* str);
int freadline_into_cstr(FILE* input, rsw_cstr* str);

int freadstring_into_new_cstr(FILE* input, int delim, rsw_cstr* str);
int freadline_into_new_cstr(FILE* input, rsw_cstr* str);


#ifdef __cplusplus
}
#endif

#endif


