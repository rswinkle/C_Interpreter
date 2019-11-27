#include <stdio.h>



int main()
{

	char a = 1;
	short b = 1;
	int c;
	double d;

	int array[5];

	/*
	printf("%ld\n", sizeof((short)a));
	printf("%ld\n", sizeof array);
	printf("%ld\n", sizeof(array));
	printf("%ld\n", sizeof(*array));
	*/

	printf("%ld\n", sizeof a);
	printf("%ld\n", sizeof b);
	printf("%ld\n", sizeof c);
	printf("%ld\n", sizeof d);

	printf("%ld\n", sizeof(a));
	printf("%ld\n", sizeof(b));
	printf("%ld\n", sizeof(c));
	printf("%ld\n", sizeof(d));

	/*
	 * error, not allowed without ()
	printf("%ld\n", sizeof char);
	printf("%ld\n", sizeof short);
	printf("%ld\n", sizeof int);
	printf("%ld\n", sizeof double);
	*/

	printf("%ld\n", sizeof(char));
	printf("%ld\n", sizeof(short));
	printf("%ld\n", sizeof(int));
	printf("%ld\n", sizeof(double));

	// 4 ie sizeof(int)
	// 5 ie sizeof(int) + 1
	// 3 ie sizeof(short) + 1
	printf("%ld\n", sizeof (1+b));
	printf("%ld\n", sizeof 100+b);
	printf("%ld\n", sizeof b+1);

	// gcc and clang both report 1 and only give a warning
	// with -pedantic (no warning even with -Wall)
	//
	// It's non-standard but makes sense as people often use
	// void* for untyped arrays (ie byte arrays) instead of 
	// using char* or uint8_t* like they should
	printf("%ld\n", sizeof(void));

	return 0;
}
