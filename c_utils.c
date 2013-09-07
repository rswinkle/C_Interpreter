#include "c_utils.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <assert.h>

/** Useful utility function since strdup isn't in standard C.*/
char* mystrdup(const char* str)
{
	/* if (!str)
	 * 	return NULL;
	 */
	size_t len = strlen(str);
	char* temp = calloc(len+1, sizeof(char));
	if (!temp) {
		return NULL;
	}

	return memcpy(temp, str, len);  /* memcpy returns to, and calloc already nulled last char */
}

c_array init_c_array(byte* data, size_t elem_size, size_t len)
{
	c_array a = { NULL, 1, 0 };
	a.data = malloc(len * elem_size + 1);
	if (!data)
		return a;

	memcpy(a.data, data, len*elem_size);
	a.data[len*elem_size] = 0;

	a.elem_size = elem_size;
	a.len = len;
	return a;
}

c_array copy_c_array(c_array src)
{
	c_array a = { NULL, 1, 0 };
	a.data = malloc(src.len * src.elem_size + 1);
	if (!a.data)
		return a;

	memcpy(a.data, src.data, src.len*src.elem_size+1); /*copy over the null byte too*/
	return a;
}


int file_open_read(const char* filename, const char* mode, c_array* out)
{
	FILE *file = fopen(filename, mode);
	if (!file)
		return 0;

	return file_read(file, out);
}



int file_read(FILE* file, c_array* out)
{
	byte* data;
	size_t size;
	out->data = NULL;
	out->len = 0;
	out->elem_size = 1;

	fseek(file, 0, SEEK_END);
	size = ftell(file);
	if (!size) {
		fclose(file);
		return 0;
	}

	data = malloc(size+1);
	if (!data) {
		fclose(file);
		return 0;
	}

	rewind(file);
	if (!fread(data, size, 1, file)) {
		printf("read failure\n");
		fclose(file);
		free(data);
		return 0;
	}

	data[size] = 0; /* null terminate in all cases even if reading binary data */

	out->data = data;
	out->len = size;
	out->elem_size = 1;

	fclose(file);
	return size;
}



int file_open_write(const char* filename, const char* mode, c_array* out)
{
	FILE* file = fopen(filename, mode);
	if (!file)
		return 0;

	return file_write(file, out);
}

int file_write(FILE* file, c_array* out)
{
	int ret = fwrite(out->data, out->elem_size, out->len, file);
	fclose(file);
	return (ret == out->len);
}









/*
 * builds an array of pointers to the strings in file_contents.  file_contents->data
 * is output from file_read or file_open_read or similar block of text/data.  The pointers
 * in lines are not allocated but point into file_contents->data ie file_contents->data
 * is modified to turn '\n' to '\0'
 */
int file_open_readlines(const char* filename, c_array* lines, c_array* file_contents)
{
	int i, pos, len;
	char** char_ptr = NULL;
	char* nl = NULL;

	lines->data = NULL;
	lines->len = 0;
	lines->elem_size = 1;

	if (!file_open_read(filename, "r", file_contents)) {
		return 0;
	}

	len = file_contents->len / 60 + 1; /* start with conservative estimate if # of lines */
	lines->data = malloc(len * sizeof(char*) + 1);
	if (!lines->data)
		return 0;

	char_ptr = (char**)lines->data;
	i = 0, pos = 0;
	while (1) {
		char_ptr[i] = (char*)&file_contents->data[pos];
		nl = strchr((char*)&file_contents->data[pos], '\n');
		if (nl) {
			*nl = '\0';
			pos = nl - (char*)file_contents->data + 1;
			i++;
			if (i == len) {
				len *= 2;
				if (!(char_ptr = realloc(lines->data, len * sizeof(char*) + 1))) {
					free(lines->data);
					lines->len = 0;
					return 0;
				}
				lines->data = (byte*)char_ptr;
			}
		} else {
			break;
		}
	}

	lines->data = realloc(char_ptr, i*sizeof(char*)+1);
	lines->len = i;
	lines->elem_size = sizeof(char*);

	return 1;
}


int freadline_into_str(FILE* input, char* str, size_t len)
{
	return freadstring_into_str(input, '\n', str, len);
}

int freadstring_into_str(FILE* input, int delim, char* str, size_t len)
{
	int temp;
	int i=0;

	if (feof(input))
		return 0;

	while (i < len-1) {
		temp = getc(input);

		if (temp == EOF || temp == delim) {
			if (!i && temp == EOF) {
				return 0;
			}
			break;
		}

		str[i] = temp;
		i++;
	}
	str[i] = '\0';


	return 1;
}

char* freadline(FILE* input)
{
	return freadstring(input, '\n', 0);
}

char* freadstring(FILE* input, int delim, size_t max_len)
{
	char* string = NULL, *tmp_str = NULL;
	int temp;
	int i=0;
	int inf = 0;

	if (feof(input))
		return NULL;

	if (!max_len) {
		inf = 1;
		max_len = 4096;
	}

	string = malloc(max_len+1);
	while (1) {
		temp = fgetc(input);

		if (temp == EOF || temp == delim) {
			if (!i && temp != delim) { //allow for delim == EOF
				free(string);
				return NULL;
			}
			string = realloc(string, i+1);
			break;
		}

		if (i == max_len) {
			if (inf) {
				tmp_str = realloc(string, max_len*2+1);
				if (!tmp_str) {
					free(string);
					return NULL;
				}
				string = tmp_str;
			} else {
				break;
			}
		}

		string[i] = temp;
		i++;
	}
	string[i] = '\0';


	return string;
}


int fpeek(FILE* input)
{
	int tmp = getc(input);
	ungetc(tmp, input);
	return tmp;
}

int readline_into_str(c_array* input, char* str, size_t len)
{
	return readstring_into_str(input, '\n', str, len);
}

int readstring_into_str(c_array* input, char delim, char* str, size_t len)
{
	char temp;
	int i=0;
	char* p = (char*) input->data;

	if (!(input->len * input->elem_size))
		return 0;

	while (*p && i < len-1) {
		temp = *p++;

		if (temp == delim) {
			break;
		}

		str[i] = temp;
		i++;
	}
	str[i] = '\0';

	return 1;
}

char* readline(c_array* input)
{
	return readstring(input, '\n', 0);
}

char* readstring(c_array* input, char delim, size_t max_len)
{
	char* string = NULL, *tmp_str = NULL;
	char temp;
	int i=0;
	int inf = 0;
	char* p = (char*) input->data;

	if (!(input->len * input->elem_size))
		return NULL;

	if (!max_len) {
		inf = 1;
		max_len = 4096;
	}

	string = malloc(max_len+1);
	while (*p) {
		temp = *p++;

		if (temp == delim) {
			if (!i) {
				free(string);
				return NULL;
			}
			string = realloc(string, i+1);
			break;
		}

		if (i == max_len) {
			if (inf) {
				tmp_str = realloc(string, max_len*2+1);
				if (!tmp_str) {
					free(string);
					return NULL;
				}
				string = tmp_str;
			} else {
				break;
			}
		}

		string[i] = temp;
		i++;
	}
	string[i] = '\0';


	return string;
}



c_array slice_c_array(c_array array, int start, int end)
{
	c_array a;
	int len;

	a.data = NULL;
	a.len = 0;

	if (start < 0)
		start = array.len + start;

	if (end < 0)
		end = array.len + end;

	if (start < 0)
		start = 0;

	if (end < 0)
		end = 0;

	if (end <= start)
		return a;

	len = end - start;
	if (!(a.data = malloc(len * array.elem_size + 1)))
		return a;

	a.data[len * array.elem_size] = 0;  /* as with file read functions always null terminate */

	memcpy(a.data, &array.data[start], len*array.elem_size);
	a.len = len;
	return a;
}

/* TODO make skip_chars byte* or u8*? */
int read_char(FILE* input, char* skip_chars, int complement, int clear_line)
{
	int c, ret;
	byte tmp;
	c_array skip;
	char* tmp_skip = (skip_chars) ? skip_chars : "";

	SET_C_ARRAY(skip, (byte*)skip_chars, 1, strlen(tmp_skip));

	do {
		ret = getc(input);
		if (ret == EOF)
			return ret;
		tmp = ret;
		c = is_any(&skip, &tmp, are_equal_uchar);
	} while (!complement && c || complement && !c);

	if (clear_line && ret != '\n')
		do { c = getc(input); } while (c != '\n' && c != EOF);

	return ret;
}

char* read_string(FILE* file, char* skip_chars, int delim, size_t max_len)
{
	int tmp;
	byte tmp2;
	char* str = NULL;
	c_array skip;
	str = (skip_chars) ? skip_chars : "";

	SET_C_ARRAY(skip, (byte*)skip_chars, 1, strlen(str));

	do {
		tmp = getc(file);
		if (tmp == EOF)
			return NULL;
		tmp2 = tmp;
	} while (is_any(&skip, &tmp2, are_equal_uchar));
	ungetc(tmp, file);
	do {
		str = freadstring(file, delim, max_len);
	} while (!str);

	return str;
}


int split(c_array* array, byte* delim, size_t delim_len, c_array* out)
{
	size_t pos = 0, max_len = 1000;
	out->elem_size = sizeof(c_array);
	out->len = 0;
	byte* match;
	c_array* results;

	out->data = malloc(max_len*sizeof(c_array)+1);
	if (!out->data)
		return 0;

	results = (c_array*)out->data;

	while (match = memchr(&array->data[pos], delim[0], array->len*array->elem_size - pos)) {
		if (!memcmp(match, delim, delim_len)) {
			results[out->len].data = &array->data[pos];
			results[out->len].elem_size = 1;
			results[out->len].len = match - &array->data[pos];

			out->len++;
			if (out->len == max_len) {
				max_len *= 2;
				out->data = realloc(results, max_len*out->elem_size + 1);
				if (!out->data) {
					free(results);
					out->data = NULL;
					return 0;
				}
				results = (c_array*)out->data;
			}
			pos = match - array->data + delim_len;
		} else {
			pos = match - array->data + 1;
		}
	}
	/* get remaining data if necessary */
	if (array->len - pos) {
		results[out->len].data = &array->data[pos];
		results[out->len].elem_size = 1;
		results[out->len].len = array->len - pos;
		out->len++;
	}

	out->data = realloc(out->data, out->len*out->elem_size+1);
	out->data[out->len*out->elem_size] = 0;

	return 1;
}


char* ltrim(char* str)
{
	int i = 0;
	int len = strlen(str);
	while (isspace(str[i]))
		i++;
	memmove(str, &str[i], len-i);
	str[len-i] = 0;
	return str;
}

char* rtrim(char* str)
{
	int i;
	int len = strlen(str);
	i = len - 1;
	while (isspace(str[i]))
		i--;
	str[++i] = 0;
	return str;
}

char* trim(char* str)
{
	return ltrim(rtrim(str));
}


#define ALPHABET_SIZE 256

/* helper function for DFA, KMP, and Boyer-Moore algorithms */
unsigned int * compute_prefix_func(byte *p, size_t len)
{
	unsigned int *pi = malloc(len*sizeof(unsigned int)); assert(pi);
	unsigned int k = 0;
	pi[0] = k;

	for (int q=1; q<len; q++) {
		while (k > 0 && p[k] != p[q])
			k = pi[k-1];
		if (p[k] == p[q])
			k++;
		pi[q] = k;
	}
	for (int i=0; i<len; ++i)
		printf("%u ", pi[i]);
	printf(" ---\n");
	return pi;
}

void prepare_badcharacter_heuristic(const byte *str, size_t str_len, int result[ALPHABET_SIZE])
{
	size_t i;

	for (i = 0; i < ALPHABET_SIZE; ++i)
		result[i] = -1;

	for (i = 0; i < str_len; ++i)
		result[str[i]] = i;
}

/* result is array of size size+1 */
void prepare_goodsuffix_heuristic(const byte *str, size_t str_len, int result[])
{
	byte *reversed;
	size_t i,j;
	char test;

	reversed = (byte*) malloc(str_len);  assert(reversed);

	/* reverse string */
	for (i=0; i<str_len; ++i)
		reversed[i] = str[str_len-1-i];

	//int prefix_normal[size];
	unsigned int *prefix_reversed = compute_prefix_func(reversed, str_len);
	free(reversed);

	//compute_prefix(normal, size, prefix_normal);
	//compute_prefix(reversed, size, prefix_reversed);

	/* can't figure out how to handle position 0 with the rest
	it's algorithm is slightly different */
	i = 1;
	result[str_len] = 1;
	while (i<str_len && prefix_reversed[i++]) {
		printf("i= %lu\n", i);
		result[str_len]++;
	}

	for (i=1; i<str_len; ++i) {
		/*max = 0; */
		test = 0;
		for (j=i; j<str_len-1; ++j) {
			/*if (!test && prefix_reversed[j] == i) */

			if (prefix_reversed[j] == i) {
				test = 1;
				if (prefix_reversed[j+1] == 0) {
					test = 2;
					break;
				}
			}
			/* if (prefix_reversed[j] > max) max++; */
		}

		if (test == 1)	/*j == str_len-1 && test) */
			result[str_len-i] = str_len;
		else if (test == 2)
			result[str_len-i] = j+1 - i;
		else
			result[str_len-i] = str_len - prefix_reversed[str_len-1];

		printf("i= %u test = %d result[str_len-i] = %d\n", (unsigned int)i, test, result[str_len-i]);
	}

	//result of 0 will only be accessed when we find a match
	//so it stores the good suffix skip of the first character
	//(last in reverse calculation)
	result[0] = str_len - prefix_reversed[str_len-1];
	//The last value in the prefix calculation is always
	//the same for a string in both directions

	free(prefix_reversed);
}
/*
 * Boyer-Moore search algorithm
 */
void boyermoore_search(c_array haystack_array, c_array needle_array)
{
	/* Calc string sizes */
	size_t needle_len, haystack_len;
	needle_len = needle_array.len;
	haystack_len = haystack_array.len;

	byte* haystack = haystack_array.data;
	byte* needle = needle_array.data;

	printf("needle length = %lu\n", needle_len);
	for (int i=0; i<needle_len; ++i)
		printf("%d ", needle[i]);
	printf("\n\n");

	/** Simple checks */
	if(haystack_len == 0)
		return;
	if(needle_len == 0)
		return;
	if(needle_len > haystack_len)
		return;

	printf("boyer_moore search\n");
	/** Initialize heuristics */
	int badcharacter[ALPHABET_SIZE];
	int* goodsuffix = malloc((needle_len+1)*sizeof(int)); assert(goodsuffix);

	prepare_badcharacter_heuristic(needle, needle_len, badcharacter);
	prepare_goodsuffix_heuristic(needle, needle_len, goodsuffix);

	/** Boyer-Moore search */
	size_t s = 0, j = 0;
	while (s <= (haystack_len - needle_len)) {
		j = needle_len;
		if (s > 6250 && s < 6260) {
			while (j > 0 && needle[j-1] == haystack[s+j-1]) {
				printf("%lu: %d %d\n",s+j-1, needle[j-1], haystack[s+j-1]);
				j--;
			}
			printf("=%lu: %d %d\n", s+j-1,  needle[j-1], haystack[s+j-1]);

		} else {
			while (j > 0 && needle[j-1] == haystack[s+j-1])
				j--;
		}

		if (j > 0) {
			int k = badcharacter[haystack[s+j-1]];
			int m;
			if (s > 6250 && s < 6260)
				printf("badcharacter = %d\n", k);
			if (k < (int)j && (m = j-k-1) > goodsuffix[j]) {
				s += m;
				if (s > 6250 && s < 6260) {
					printf("badcharacter = %d\n", k);
					printf("adding m = %d\n", m);
				}
			} else {
				s += goodsuffix[j];
				if (s > 6250 && s < 6260)
					printf("adding goodsuffix[%lu] = %d\n", j, goodsuffix[j]);
			}
		} else {
			printf("Pattern found at %lu\n", s);
			s += goodsuffix[0];
		}
	}

	free(goodsuffix);
}




void basic_search(c_array haystack, c_array needle)
{
	byte* result = haystack.data;
	byte* end = haystack.data + haystack.len*haystack.elem_size;
	while(result = memchr(result, needle.data[0], end-result)) {
		if (!memcmp(result, needle.data, needle.len*needle.elem_size)) {
			printf("Pattern found at %lu\n", result-haystack.data);
			result += needle.len*needle.elem_size;
		} else {
			++result;
		}
	}
}


void* mybsearch(const void *key, const void *buf, size_t num, size_t size, int (*compare)(const void *, const void *))
{
	size_t min = 0, max = num-1;
	size_t cursor;

	while (min <= max) {
		cursor = min + ((max - min) / 2);
		int ret = compare(&key, (const char*)buf+cursor*size);
		if (!ret) {
			return (char*)buf + cursor*size;
		} else if (ret < 0) {
			max = cursor - 1;  //overflow possibilities here and below
		} else {
			min = cursor + 1;
		}
	}
	return NULL;
}



int compare_char(const void* a, const void* b)
{
	char a_ = *(char*)a;
	char b_ = *(char*)b;

	if (a_ < b_)
		return -1;
	else if (a_ > b_)
		return 1;

	return 0;
}

int compare_uchar(const void* a, const void* b)
{
	unsigned char a_ = *(unsigned char*)a;
	unsigned char b_ = *(unsigned char*)b;

	if (a_ < b_)
		return -1;
	else if (a_ > b_)
		return 1;

	return 0;
}

int compare_short(const void* a, const void* b)
{
	short a_ = *(short*)a;
	short b_ = *(short*)b;

	if (a_ < b_)
		return -1;
	else if (a_ > b_)
		return 1;

	return 0;
}

int compare_ushort(const void* a, const void* b)
{
	unsigned short a_ = *(unsigned short*)a;
	unsigned short b_ = *(unsigned short*)b;

	if (a_ < b_)
		return -1;
	else if (a_ > b_)
		return 1;

	return 0;
}


int compare_int(const void* a, const void* b)
{
	int a_ = *(int*)a;
	int b_ = *(int*)b;

	if (a_ < b_)
		return -1;
	else if (a_ > b_)
		return 1;

	return 0;
}

int compare_uint(const void* a, const void* b)
{
	unsigned int a_ = *(unsigned int*)a;
	unsigned int b_ = *(unsigned int*)b;

	if (a_ < b_)
		return -1;
	else if (a_ > b_)
		return 1;

	return 0;
}

int compare_long(const void* a, const void* b)
{
	long a_ = *(long*)a;
	long b_ = *(long*)b;

	if (a_ < b_)
		return -1;
	else if (a_ > b_)
		return 1;

	return 0;
}

int compare_ulong(const void* a, const void* b)
{
	unsigned long a_ = *(unsigned long*)a;
	unsigned long b_ = *(unsigned long*)b;

	if (a_ < b_)
		return -1;
	else if (a_ > b_)
		return 1;

	return 0;
}

int compare_float(const void* a, const void* b)
{
	float a_ = *(float*)a;
	float b_ = *(float*)b;

	if (a_ < b_)
		return -1;
	else if (a_ > b_)
		return 1;

	return 0;
}

int compare_double(const void* a, const void* b)
{
	double a_ = *(double*)a;
	double b_ = *(double*)b;

	if (a_ < b_)
		return -1;
	else if (a_ > b_)
		return 1;

	return 0;
}







int are_equal_char(const void* a, const void* b)
{
	return *(char*)a == *(char*)b;
}

int are_equal_uchar(const void* a, const void* b)
{
	return *(unsigned char*)a == *(unsigned char*)b;
}

int are_equal_short(const void* a, const void* b)
{
	return *(short*)a == *(short*)b;
}

int are_equal_ushort(const void* a, const void* b)
{
	return *(unsigned short*)a == *(unsigned short*)b;
}

int are_equal_int(const void* a, const void* b)
{
	return *(int*)a == *(int*)b;
}

int are_equal_uint(const void* a, const void* b)
{
	return *(unsigned int*)a == *(unsigned int*)b;
}

int are_equal_long(const void* a, const void* b)
{
	return *(long*)a == *(long*)b;
}

int are_equal_ulong(const void* a, const void* b)
{
	return *(unsigned long*)a == *(unsigned long*)b;
}

int are_equal_float(const void* a, const void* b)
{
	return *(float*)a == *(float*)b;
}
int are_equal_double(const void* a, const void* b)
{
	return *(double*)a == *(double*)b;
}


//int is_any_array


int is_any(c_array* array, const void* the_one, int (*are_equal)(const void*, const void*))
{
	size_t i;
	for (i=0; i<array->len; ++i) {
		if (are_equal(the_one, &array->data[i*array->elem_size]))
			return 1;
	}
	return 0;
}




int any(c_array* array, int (*is_true)(const void*))
{
	size_t i;
	for (i=0; i<array->len; ++i) {
		if (is_true(&array->data[i*array->elem_size])) {
			return 1;
		}
	}
	return 0;
}


int all(c_array* array, int (*is_true)(const void*))
{
	size_t i;
	for (i=0; i<array->len; ++i) {
		if (!is_true(&array->data[i*array->elem_size])) {
			return 0;
		}
	}
	return 1;
}


void map(c_array* array, void (*func)(const void*))
{
	size_t i;
	for (i=0; i<array->len; ++i) {
		func(&array->data[i*array->elem_size]);
	}
}




