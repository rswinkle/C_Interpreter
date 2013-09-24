#include <stdio.h>

#define BINARY_OP1(a, b, c) (a b c)

#define MYCOMMA ,

int global = 1.5;


int main()
{
	printf("%d\n", global);


	double d = 5.5;

	//int a = 10 % (d * 2);

	printf("%f\n", d);

	printf("%d\n", BINARY_OP1(5, +, 2));
	printf("%d\n", BINARY_OP1(5, -, 2));
	printf("%d\n", BINARY_OP1(5, *, 2));
	printf("%d\n", BINARY_OP1(5, /, 2));

	if ((char)-1 < (unsigned int)100)
		printf("true\n");
	int one = 1, zero = 0;

	if (BINARY_OP1(one, &&, zero))
		printf("and true\n");

	if (BINARY_OP1(one, ||, zero))
		printf("or true\n");

	if (BINARY_OP1(one, >, zero))
		printf("> true\n");

	if (BINARY_OP1(one, <=, zero))
		printf("<= true\n");

	printf("%d\n", (BINARY_OP1(zero, MYCOMMA, one)));

	int b = BINARY_OP1(zero, MYCOMMA, one);
	printf("%d\n", b);


	if ((int)(d = 0.50))
		printf("double true\n");

	b = d;
	if (b)
		printf("int true\n");

	printf("%d\n\n\n", (d = 0.5) ? 1 : 0);

	printf("%f\n", d);
	d += 3;
	printf("%f\n", d);

	#define PRE_OP(A, B) A B
	int test = 0;
	printf("%d\n", PRE_OP(++, test));
	printf("%d\n", PRE_OP(--, test));


	int test2 = (d > 2) ? 1.5 : 3; 
	printf("%d\n", test2);

	char letter = 'ab';
	printf("%c %d\n", letter, letter);

	switch (test2) {
		//tcc allows this ...
		//gcc error case does not reduce to integer constant
		//case 1.5:
		case 1:
			printf("am I here\n");
			break;
	}

	return 0;
}
