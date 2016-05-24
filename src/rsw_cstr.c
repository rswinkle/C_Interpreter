#include "rsw_cstr.h"

#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <assert.h>

#define STDERR(X) fprintf(stderr, X)

#define RSW_CSTR_ALLOCATOR(x) ((x) * 2)


size_t CSTR_ST_SZ = 25;


//vector_char* vec_char_heap(size_t size, size_t capacity);
//vector_char* init_vec_char_heap(char* vals, size_t num);

int init_cstr(rsw_cstr* str)
{
	str->size = 0;
	str->capacity = CSTR_ST_SZ;

	if (!(str->a = (char*)malloc(str->capacity*sizeof(char)))) {
		assert(str->a != NULL);
		str->size = str->capacity = 0;
		return 0;
	}
	str->a[0] = 0;

	return 1;
}

int init_cstr_cap(rsw_cstr* str, size_t capacity)
{
	str->size = 0;
	str->capacity = (capacity > 0) ? capacity : CSTR_ST_SZ;

	if (!(str->a = (char*)malloc(str->capacity*sizeof(char)))) {
		assert(str->a != NULL);
		str->capacity = 0;
		return 0;
	}
	str->a[0] = 0;

	return 1;
}

int init_cstr_sz(rsw_cstr* str, size_t size, int val)
{
	str->size = size;
	str->capacity = size + CSTR_ST_SZ;

	if (!(str->a = (char*)malloc(str->capacity*sizeof(char)))) {
		assert(str->a != NULL);
		str->size = str->capacity = 0;
		return 0;
	}

	memset(str->a, val, size);
	str->a[size] = 0;

	return 1;
}

int init_cstr_sz_cap(rsw_cstr* str, size_t size, int val, size_t capacity)
{
	str->size = size;
	str->capacity = (capacity > size) ? capacity : size + CSTR_ST_SZ;

	if (!(str->a = (char*)malloc(str->capacity*sizeof(char)))) {
		assert(str->a != NULL);
		str->size = str->capacity = 0;
		return 0;
	}

	memset(str->a, val, size);
	str->a[size] = 0;

	return 1;
}


int init_cstr_str(rsw_cstr* str, char* start, size_t len)
{
	str->capacity = len + CSTR_ST_SZ;
	str->size = len;
	if (!(str->a = (char*)malloc(str->capacity*sizeof(char)))) {
		assert(str->a != NULL);
		str->size = str->capacity = 0;
		return 0;
	}

	memcpy(str->a, start, sizeof(char)*len);
	str->a[len] = 0;

	return 1;
}

/** Assumes dest is initialized */
void cstr_copy(void* dest, void* src)
{
	rsw_cstr* str1 = (rsw_cstr*)dest;
	rsw_cstr* str2 = (rsw_cstr*)src;
	
	if (!cstr_reserve(str1, str2->size))
		return;
	
	memcpy(str1->a, str2->a, str2->size*sizeof(char));
	str1->size = str2->size;
	str1->a[str1->size] = 0;
}


int cstr_push(rsw_cstr* str, char a)
{
	char* tmp;
	size_t tmp_sz;
	if (str->capacity > str->size + 1) {
		str->a[str->size++] = a;
		str->a[str->size] = 0;
	} else {
		tmp_sz = RSW_CSTR_ALLOCATOR(str->capacity);
		if (!(tmp = (char*)realloc(str->a, sizeof(char)*tmp_sz))) {
			assert(tmp != NULL);
			return 0;
		}
		str->a = tmp;
		str->a[str->size++] = a;
		str->a[str->size] = 0;
		str->capacity = tmp_sz;
	}
	return 1;
}



char cstr_pop(rsw_cstr* str)
{
	char tmp = str->a[--str->size];
	str->a[str->size] = 0;
	return tmp;
}


int cstr_extend(rsw_cstr* str, size_t num, char a)
{
	char* tmp;
	size_t tmp_sz;
	if (str->capacity < str->size + num + 1) {
		tmp_sz = str->capacity + num + CSTR_ST_SZ;
		if (!(tmp = (char*)realloc(str->a, sizeof(char)*tmp_sz))) {
			assert(tmp != NULL);
			return 0;
		}
		str->a = tmp;
		str->capacity = tmp_sz;
	}

	memset(&str->a[str->size], a, num);
	str->size += num;
	str->a[str->size] = 0;
	return 1;
}


//insert will insert at the end (str->size) behaving just like push
int cstr_insert(rsw_cstr* str, size_t i, char a)
{
	char* tmp;
	size_t tmp_sz;
	if (str->capacity > str->size + 1) {
		memmove(&str->a[i+1], &str->a[i], (str->size-i)*sizeof(char));
		str->a[i] = a;
	} else {
		tmp_sz = RSW_CSTR_ALLOCATOR(str->capacity);
		if (!(tmp = (char*)realloc(str->a, sizeof(char)*tmp_sz))) {
			assert(tmp != NULL);
			return 0;
		}
		str->a = tmp;
		memmove(&str->a[i+1], &str->a[i], (str->size-i)*sizeof(char));
		str->a[i] = a;
		str->capacity = tmp_sz;
	}

	str->size++;
	str->a[str->size] = 0;
	return 1;
}


//The insert functions will work inserting at the end ie at str->size
//in which case they behave just like the concatenate functions
int cstr_insert_str(rsw_cstr* str, size_t i, char* a, size_t len)
{
	char* tmp;
	size_t tmp_sz;
	if (str->capacity < str->size + len + 1) {
		tmp_sz = str->capacity + len + CSTR_ST_SZ;
		if (!(tmp = (char*)realloc(str->a, sizeof(char)*tmp_sz))) {
			assert(tmp != NULL);
			return 0;
		}
		str->a = tmp;
		str->capacity = tmp_sz;
	}

	memmove(&str->a[i+len], &str->a[i], (str->size-i)*sizeof(char));
	memcpy(&str->a[i], a, len*sizeof(char));
	str->size += len;
	str->a[str->size] = 0;
	return 1;
}

int cstr_insert_cstr(rsw_cstr* str, size_t i, rsw_cstr* a_str)
{
	char* tmp;
	size_t tmp_sz;
	size_t len = a_str->size;
	if (str->capacity < str->size + len + 1) {
		tmp_sz = str->capacity + len + CSTR_ST_SZ;
		if (!(tmp = (char*)realloc(str->a, sizeof(char)*tmp_sz))) {
			assert(tmp != NULL);
			return 0;
		}
		str->a = tmp;
		str->capacity = tmp_sz;
	}

	memmove(&str->a[i+len], &str->a[i], (str->size-i)*sizeof(char));
	memcpy(&str->a[i], a_str->a, len*sizeof(char));
	str->size += len;
	str->a[str->size] = 0;
	return 1;
}

int cstr_concatenate(rsw_cstr* str, char* a, size_t len)
{
	char* tmp;
	size_t tmp_sz;
	if (str->capacity < str->size + len + 1) {
		tmp_sz = str->capacity + len + CSTR_ST_SZ;
		if (!(tmp = (char*)realloc(str->a, sizeof(char)*tmp_sz))) {
			assert(tmp != NULL);
			return 0;
		}
		str->a = tmp;
		str->capacity = tmp_sz;
	}

	memcpy(&str->a[str->size], a, len);
	str->size += len;
	str->a[str->size] = 0;
	return 1;
}

int cstr_concatenate_cstr(rsw_cstr* str, rsw_cstr* a_str)
{
	return cstr_concatenate(str, a_str->a, a_str->size);
}

/** Erace characters between start and end inclusive */
void cstr_erase(rsw_cstr* str, size_t start, size_t end)
{
	size_t d = end - start + 1;
	memmove(&str->a[start], &str->a[end+1], (str->size-1-end)*sizeof(char));
	str->size -= d;
	str->a[str->size] = 0;
}


int cstr_reserve(rsw_cstr* str, size_t size)
{
	char* tmp;
	if (str->capacity < size + 1) {
		if (!(tmp = (char*)realloc(str->a, sizeof(char)*(size+CSTR_ST_SZ)))) {
			assert(tmp != NULL);
			return 0;
		}
		str->a = tmp;
		str->capacity = size + CSTR_ST_SZ;
	}
	return 1;
}




int cstr_set_capacity(rsw_cstr* str, size_t size)
{
	char* tmp;
	if (size < str->size) {
		str->size = size;
	}

	if (!(tmp = (char*)realloc(str->a, sizeof(char)*(size+1)))) {
		assert(tmp != NULL);
		return 0;
	}
	str->a = tmp;
	str->capacity = size+1;
	str->a[str->size] = 0;
	return 1;
}



void cstr_set_val_sz(rsw_cstr* str, char val)
{
	size_t i;
	for (i=0; i<str->size; i++) {
		str->a[i] = val;
	}
}

//not sure when I'd use this
void cstr_set_val_cap(rsw_cstr* str, char val)
{
	size_t i;
	for (i=0; i<str->capacity-1; i++) {
		str->a[i] = val;
	}
	str->a[str->capacity-1] = 0;
}


void cstr_clear(rsw_cstr* str) { str->size = 0; }

void free_cstr_heap(void* str)
{
	rsw_cstr* tmp = (rsw_cstr*)str;
	free(tmp->a);
	free(tmp);
}

void free_cstr(void* str)
{
	rsw_cstr* tmp = (rsw_cstr*)str;
	free(tmp->a);
	tmp->size = 0;
	tmp->capacity = 0;
}

/* Sets a cstr to first num chars in a (clears previous contents) */
int cstr_set_str(rsw_cstr* str, char* a, size_t len)
{
	char* tmp;
	size_t tmp_sz;
	if (str->capacity < len + 1) {
		tmp_sz = len + CSTR_ST_SZ;
		if (!(tmp = (char*)realloc(str->a, sizeof(char)*tmp_sz))) {
			assert(tmp != NULL);
			return 0;
		}
		str->a = tmp;
		str->capacity = tmp_sz;
	}

	memcpy(str->a, a, len*sizeof(char));
	str->size = len;
	str->a[str->size] = 0;
	return 1;
}

rsw_cstr* cstr_ltrim(rsw_cstr* str)
{
	size_t i = 0;

	while (isspace(str->a[i++]));

	memmove(str->a, &str->a[i], str->size-i);
	str->size -= i;
	str->a[str->size] = 0;

	return str;
}

rsw_cstr* cstr_rtrim(rsw_cstr* str)
{
	size_t i;

	i = str->size - 1;
	while (i != (size_t)-1 && isspace(str->a[i--]));

	str->size = i + 1;
	str->a[str->size] = 0;
	return str;
}

rsw_cstr* cstr_trim(rsw_cstr* str)
{
	return cstr_ltrim(cstr_rtrim(str));
}

/* returns a new string like python.  I'll have to check
 * the return string's members to know if it failed or not */
rsw_cstr slice_cstr(rsw_cstr* str, long start, long end)
{
	rsw_cstr str2 = { NULL, 0, 0 };
	int len;

	if (start < 0)
		start = str->size + start;

	if (end < 0)
		end = str->size + end;

	if (start < 0)
		start = 0;

	if (end < 0)
		end = 0;

	if (end < start)
		end = start;

	len = end - start;

	init_cstr_str(&str2, str->a, len);
	return str2;
}

rsw_cstr cstr_substr(rsw_cstr* str, size_t index, size_t len)
{
	rsw_cstr s;

	assert(index < str->size);

	len = (len > str->size - index) ? str->size-index : len;

	init_cstr_str(&s, &str->a[index], len);
	return s;
}

int cstr_resize(rsw_cstr* str, size_t size, int val)
{
	if (!cstr_reserve(str, size))
		return 0;

	if (size > str->size)
		memset(&str->a[str->size], val, size-str->size);

	str->size = size;
	str->a[size] = 0;
	return 1;
}

/* probably just use strstr directly most of the time
 * but I'm trying to parallel C++ std::string functions */
size_t cstr_find(rsw_cstr* str, rsw_cstr* needle)
{
	char* result = strstr(str->a, needle->a);
	if (result)
		return result - str->a;
	else
		return -1; /* TODO make a macro or static const size_t npos = -1 ? */
}

size_t cstr_find_start_at(rsw_cstr* str, rsw_cstr* needle, size_t start)
{
	if (start >= str->size)
		return -1;

	char* result = strstr(&str->a[start], needle->a);
	if (result)
		return result - str->a;
	else
		return -1; /* TODO make a macro or static const size_t npos = -1 ? */
}

size_t cstr_find_str(rsw_cstr* str, char* needle)
{
	char* result = strstr(str->a, needle);
	if (result)
		return result - str->a;
	else
		return -1; /* TODO make a macro or static const size_t npos = -1 ? */
}

size_t cstr_find_str_start_at(rsw_cstr* str, char* needle, size_t start)
{
	if (start >= str->size)
		return -1;

	char* result = strstr(&str->a[start], needle);
	if (result)
		return result - str->a;
	else
		return -1; /* TODO make a macro or static const size_t npos = -1 ? */
}

int cstr_replace(rsw_cstr* str, char* find, char* a, size_t count)
{
	size_t find_len = strlen(find);
	size_t a_len = strlen(a);
	long len_added = a_len - find_len;
	size_t index;

	if (count == 0)
		count = -1; /* converted to max value */


	char* result = strstr(str->a, find);
	while (result && count > 0) {
		index = result - str->a;

		if (len_added > 0 && !cstr_reserve(str, str->size + len_added))
			return 0;

		if (len_added)
			memmove(&str->a[index+a_len], &str->a[index+find_len], str->size-(index+find_len));

		memcpy(&str->a[index], a, a_len);
		str->size += len_added;
		str->a[str->size] = 0;
		--count;
		result = strstr(&str->a[index+a_len], find);
	}

	return 1;
}

//replace up to num characters of str starting at index with all of str2
//so this can cause the string to grow or shrink or stay the same size
int cstr_replace_substr_cstr(rsw_cstr* str, size_t index, size_t num, rsw_cstr* str2)
{
	return cstr_replace_substr(str, index, num, str2->a, str2->size);
}

int cstr_replace_substr(rsw_cstr* str, size_t index, size_t num, char* a, size_t len)
{
	if (index >= str->size) {
		assert(index < str->size);
		return 0;
	}

	if (num == (size_t)-1 || index + num > str->size)
		num = str->size - index;

	long len_added = len - num;

	if (len_added > 0 && !cstr_reserve(str, str->size + len_added))
		return 0;

	//could leave out the check ... it'd just copy the elements to where they already were
	if (len_added)
		memmove(&str->a[index+len], &str->a[index+num], str->size-(index+num));

	memcpy(&str->a[index], a, len);

	str->size += len_added;
	str->a[str->size] = 0;
	return 1;
}

int cstr_split(rsw_cstr* str, rsw_cstr* delim, rsw_cstr** results, size_t* num_results)
{
	if (!results || !num_results)
		return 0;

	size_t num = 0;
	
	rsw_cstr* ret, *tmp;
	if (delim->size == 0) {
		if (!(ret = (rsw_cstr*)malloc(sizeof(rsw_cstr)))) {
			assert(ret);
			return 0;
		}
		num = 1;
		init_cstr_str(ret, str->a, str->size);
		return 1;
	}

	int cap = 10;
	ret = (rsw_cstr*)malloc(sizeof(rsw_cstr)*cap);
	
	char* search = str->a;
	char* result;
	while (result = strstr(search, delim->a)) {
		init_cstr_str(&ret[num], search, result-search);
		++num;

		//since there's
		if (num == cap) {
			if (!(tmp = (rsw_cstr*) realloc(ret, sizeof(rsw_cstr)*cap*2))) {
				assert(tmp);
				free(ret);
				num = 0;
				return 0;
			}
			ret = tmp;
			cap *= 2;
		}
		search = result + delim->size;
	}

	//if the last thing in the string was the delimiter then search will be '\0'
	//else put everything left in one last result
	if (*search) {
		init_cstr_str(&ret[num], search, str->size-(search-str->a));
		++num;
	}

	if (!(tmp = (rsw_cstr*) realloc(ret, sizeof(rsw_cstr)*num))) {
		assert(tmp);
		free(ret);
		num = 0;
		return 0;
	}

	*num_results = num;
	*results = tmp;
	return 1;
}


int file_open_read_cstr(const char* filename, rsw_cstr* str)
{
	FILE *file = fopen(filename, "r");
	if (!file)
		return 0;

	return file_read_cstr(file, str);
}

int file_open_read_new_cstr(const char* filename, rsw_cstr* str)
{
	FILE *file = fopen(filename, "r");
	if (!file)
		return 0;

	return file_read_new_cstr(file, str);
}

//assumes str already initialized
int file_read_cstr(FILE* file, rsw_cstr* str)
{
	long size;

	fseek(file, 0, SEEK_END);
	size = ftell(file);
	if (size <= 0) {
		if (size == -1)
			perror("ftell failure");
		fclose(file);
		str->size = 0;
		return 0;
	}

	if (!cstr_reserve(str, size)) {
		fclose(file);
		return 0;
	}
	
	rewind(file);
	if (!fread(str->a, size, 1, file)) {
		perror("fread failure");
		fclose(file);
		return 0;
	}

	str->size = size;
	str->a[str->size] = 0;

	fclose(file);
	return size;
}

//str is not initialized
int file_read_new_cstr(FILE* file, rsw_cstr* str)
{
	long size;

	fseek(file, 0, SEEK_END);
	size = ftell(file);
	if (size <= 0) {
		if (size == -1)
			perror("ftell failure");
		fclose(file);
		str->size = 0;
		return 0;
	}

	//allocate just enough, maybe change later or add a parameter
	if (!init_cstr_cap(str, size+1)) {
		fclose(file);
		return 0;
	}
	
	rewind(file);
	if (!fread(str->a, size, 1, file)) {
		perror("fread failure");
		fclose(file);
		return 0;
	}

	str->size = size;
	str->a[str->size] = 0;

	fclose(file);
	return size;
}



int file_open_write_cstr(const char* filename, rsw_cstr* str)
{
	FILE* file = fopen(filename, "w");
	if (!file)
		return 0;

	return file_write_cstr(file, str);
}

int file_write_cstr(FILE* file, rsw_cstr* str)
{
	int ret = fwrite(str->a, 1, str->size, file);
	fclose(file);
	return (ret == str->size);
}


int file_open_readlines_cstr(const char* filename, rsw_cstr** lines, size_t* num_results)
{
	rsw_cstr filedata, delim;

	if (!num_results)
		return 0;

	if (!file_open_read_new_cstr(filename, &filedata)) {
		*num_results = 0;
		return 0;
	}

	//seems dumb to malloc 2 bytes .. I should make a split function that
	//only splits on single character delimiters, passed in as a char not a cstr or a char*
	delim.a = "\n";
	delim.size = 1;
	delim.capacity = 2;

	if (!cstr_split(&filedata, &delim, lines, num_results)) {
		free_cstr(&filedata);
		return 0;
	}

	return 1;
}

//assumes str is initialized, returns # chars added or 0 on error
int freadstring_into_cstr(FILE* input, int delim, rsw_cstr* str)
{
	int temp;
	int i=0;

	if (feof(input))
		return 0;

	while (1) {
		temp = getc(input);

		if (temp == EOF || temp == delim) {
			if (!i && temp != delim) {
				return 0;
			}
			break;
		}

		cstr_push(str, temp);
		++i;
	}

	str->a[str->size] = '\0';
	return i;
}

int freadline_into_cstr(FILE* input, rsw_cstr* str)
{
	return freadstring_into_cstr(input, '\n', str);
}

int freadstring_into_new_cstr(FILE* input, int delim, rsw_cstr* str)
{
	int temp;
	int i=0;

	//I figure 99 is long enough to handle most delimited strings
	//without realloc'ing, ie csv's
	if (!init_cstr_cap(str, 100))
		return 0;

	if (feof(input))
		return 0;

	while (1) {
		temp = getc(input);

		if (temp == EOF || temp == delim) {
			if (!i && temp != delim) {
				return 0;
			}
			break;
		}

		cstr_push(str, temp);
		++i;
	}

	str->a[str->size] = '\0';
	return i;
}

int freadline_into_new_cstr(FILE* input, rsw_cstr* str)
{
	return freadstring_into_new_cstr(input, '\n', str);
}


