#include "parser.h"
#include "interpreter.h"

#include <stdio.h>




extern vector_void stmt_list;
extern FILE* file;


int main(int argc, char** argv)
{
	if (argc != 2) {
		printf("Usage: %s script\n", argv[0]);
		return 0;
	}
	file = NULL;
	file = fopen(argv[1], "r");
	if (!file) {
		perror("Error opening file");
		return 0;
	}

	vec_void(&stmt_list, 0, 40, sizeof(statement), free_statement, NULL);
	parse_program();
	run();

	free_vec_void(&stmt_list);
	fclose(file);

	return 0;
}
