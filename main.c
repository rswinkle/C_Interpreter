#include "parser.h"
#include "interpreter.h"

#include <stdio.h>



int main(int argc, char** argv)
{
	if (argc != 2) {
		printf("Usage: %s script\n", argv[0]);
		return 0;
	}
	FILE* file = NULL;
	file = fopen(argv[1], "r");
	if (!file) {
		perror("Error opening file");
		return 0;
	}

	program_state program;

	parse_program(&program, file);
	run(&program, NULL);


	return 0;
}
