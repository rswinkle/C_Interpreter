#include "preprocessor.h"
#include "parser.h"
#include "interpreter.h"

#include "c_utils.h"

#include <stdio.h>



int main(int argc, char** argv)
{
	if (argc < 2) {
		printf("Usage: %s [-E] script\n", argv[0]);
		return 0;
	}

	char* filename = (argc == 2) ? argv[1] : argv[2];

	FILE* file = NULL;
	file = fopen(filename, "r");
	if (!file) {
		perror("Error opening file");
		return 0;
	}

	program_state program;
	FILE* preprocessed = run_preprocessor(file, filename);

	if (argc == 3 && !strcmp(argv[1], "-E")) {
		c_array preproc_out;
		file_read(preprocessed, &preproc_out);
		fprintf(stdout, "%s\n", preproc_out.data);
		free(preproc_out.data);
		return 0;
	}

	parse_program_file(&program, preprocessed);
	run(&program, "main");



	return 0;
}
